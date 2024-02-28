#!/usr/bin/perl
use warnings;
use strict;
use Encode;
use File::Basename;
use File::Slurp qw(read_file);
use HTML5::DOM;
use Getopt::Long;
use WWW::Curl::Easy;
use WWW::Curl::Form;
use Data::Dumper;

my $KIBAB_LOGIN;
my $KIBAB_PASSWORD;

GetOptions(
	"login=s" => \$KIBAB_LOGIN,
	"password=s" => \$KIBAB_PASSWORD,
);

if (!$KIBAB_LOGIN || !$KIBAB_PASSWORD) {
	die "usage: $0 --login=LOGIN --password=PASSWORD_COOKIE\n";
}

my $model2patch = {};
for my $line (split(/(\r\n|\n)/, scalar(read_file(dirname(__FILE__)."/patches.txt")))) {
	my ($model, $patch) = split(/\|/, $line);
	$model2patch->{$model} = $patch;
}

for my $model (sort keys %$model2patch) {
	my $file = dirname(__FILE__)."/../release/ElfPack_$model.vkp";
	next if !-f $file;
	
	my $old_patch = $model2patch->{$model};
	
	my ($patch_id) = $old_patch =~ /id=(\d+)/;
	die "Invalid patch URL: $old_patch" if !$patch_id;
	
	# next if $patch_id != 10732;
	
	print "[$model]\n";
	print "ELFLoader: $file\n";
	print "Old patch: $old_patch\n";
	
	my $patch_data = scalar(read_file($file));
	
	my ($version) = $patch_data =~ /Version: (.*?)$/im;
	my ($date) = $patch_data =~ /Date: (.*?)$/im;
	print "Version: $version ($date)\n";
	
	die "Invalid patch\n$patch_data" if !$date || !$version;
	
	my $description = join("\n",
		"[b]${date} - v${version}[/b]",
		"Список изменений и исходники: [url]https://github.com/siemens-mobile-hacks/elfloader3/blob/master/README.md[/url]",
		"SDK для разработчиков: [url]https://github.com/siemens-mobile-hacks/sdk[/url]",
		"[b][color:darkgreen]Для багов:[/color][/b] [url]https://github.com/siemens-mobile-hacks/elfloader3/issues[/url]",
		"",
		"[EN]:",
		"[b]${date} - v${version}[/b]",
		"Changelog and sources: [url]https://github.com/siemens-mobile-hacks/elfloader3/blob/master/README.md[/url]",
		"SDK for developers: [url]https://github.com/siemens-mobile-hacks/sdk[/url]",
		"[b][color:darkgreen]Bugtracker:[/color][/b] [url]https://github.com/siemens-mobile-hacks/elfloader3/issues[/url]",
	);
	
	my $old_fields = getPatchData($patch_id);
	my $fields = { %$old_fields };
	
	$patch_data =~ s/\r\n|\n/\r\n/g;
	$patch_data =~ s/^\s+|\s+$//g;
	
	$fields->{is_beta} = 1;
	$fields->{p_ver} = $version;
	$fields->{p_name_RU} = "ElfPack [$date]";
	$fields->{p_name_EN} = "ElfPack [$date]";
	$fields->{p_shortdesc} = $description;
	$fields->{p_dname} = "ElfPack-${date}-v${version}";
	$fields->{p_dname} =~ s/[.]/_/g;
	$fields->{p_text} = $patch_data;
	
	if (checkUpdateData($old_fields, $fields)) {
		print "Updating patch...\n";
		savePatchData($fields);
	}
}

sub cmpTextStr {
	my ($a, $b) = @_;
	
	$a = "$a";
	$b = "$b";
	
	$a =~ s/\r\n|\n/\r\n/g;
	$a =~ s/^\s+|\s+$//g;
	
	$b =~ s/\r\n|\n/\r\n/g;
	$b =~ s/^\s+|\s+$//g;
	
	return $a eq $b;
}

sub checkUpdateData {
	my ($old, $new) = @_;
	
	for my $k (keys %$new) {
		return $k if !exists $old->{$k};
		return $k if !cmpTextStr($old->{$k}, $new->{$k});
	}
	
	for my $k (keys %$old) {
		return $k if !exists $new->{$k};
		return $k if !cmpTextStr($old->{$k}, $new->{$k});
	}
	
	return 0;
}

sub savePatchData {
	my ($fields) = @_;
	
	my $response;
	my $curl = WWW::Curl::Easy->new;
	$curl->setopt(CURLOPT_WRITEDATA, \$response);
	$curl->setopt(CURLOPT_COOKIE, "login=$KIBAB_LOGIN; password=$KIBAB_PASSWORD");
	$curl->setopt(CURLOPT_URL, 'https://patches.kibab.com/patches/addpatch.php5');
	
	my $form = WWW::Curl::Form->new;
	for my $k (keys %$fields) {
		die "Undefined key $k" if !defined $fields->{$k};
		
		my $v = $fields->{$k};
		Encode::from_to($v, "utf-8", "cp1251");
		$v =~ s/(\r\n|\n)/\r\n/g;
		
		$form->formadd($k, $v);
	}
	$curl->setopt(CURLOPT_POST, 1);
	$curl->setopt(CURLOPT_HTTPPOST, $form);
	
	my $ret = $curl->perform;
	my $http_code = $curl->getinfo(CURLINFO_HTTP_CODE);
	die("savePatchData(".$fields->{id}."): error: ".($curl->errbuf ? $curl->errbuf : "HTTP: $http_code")) if $ret != 0 || $http_code != 302;
}

sub getPatchData {
	my ($id) = @_;
	
	my $response;
	my $curl = WWW::Curl::Easy->new;
	$curl->setopt(CURLOPT_WRITEDATA, \$response);
	$curl->setopt(CURLOPT_COOKIE, "login=$KIBAB_LOGIN; password=$KIBAB_PASSWORD");
	$curl->setopt(CURLOPT_URL, 'https://patches.kibab.com/patches/addpatch.php5?id='.$id.'&action=edit_patch');
	
	my $ret = $curl->perform;
	my $http_code = $curl->getinfo(CURLINFO_HTTP_CODE);
	die("getPatchData(".$id."): error: ".($curl->errbuf ? $curl->errbuf : "HTTP: $http_code")) if $ret != 0 || $http_code != 200;
	
	my $doc = HTML5::DOM->new->parse($response);
	my $form = $doc->at('#PatchAddForm');
	die "getPatchData($id): form not found!" if !$form;
	
	my $fields = {};
	for my $input (@{$form->find('textarea, input, select')}) {
		my $type = $input->attr("type") // "unknown";
		next if $type eq "button" or $type eq "submit" or $type eq "file";
		next if !defined $input->attr("name");
		
		die "Duplicate: ".$input->attr("name") if exists $fields->{$input->attr("name")};
		
		if ($input->nodeName eq "SELECT") {
			$fields->{$input->attr("name")} = $input->at('[selected]')->attr("value");
		} elsif ($type eq "hidden" || $type eq "text") {
			$fields->{$input->attr("name")} = $input->attr("value");
		} elsif ($input->nodeName eq "TEXTAREA") {
			$fields->{$input->attr("name")} = $input->textContent;
		} elsif ($type eq "checkbox" || $type eq "radio") {
			$fields->{$input->attr("name")} = $input->attr("value") if $input->attr("checked");
		} else {
			die "Unknown input: ".$input;
		}
	}
	return $fields;
}

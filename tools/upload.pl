#!/usr/bin/perl
use warnings;
use strict;
use Encode;
use File::Basename;
use File::Slurp qw(read_file);
use HTML5::DOM;
use Getopt::Long;
use LWP::UserAgent;
use HTTP::Request::Common;
use HTTP::Cookies;
use Data::Dumper;
use URI;
use URI::Escape;

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
	$patch_data =~ s/^;.*?\r\n\r\n//gsi;
	$patch_data =~ s/^\s+|\s+$//g;
	$patch_data .= "\r\n";

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

	my $ua = LWP::UserAgent->new;
	$ua->cookie_jar(HTTP::Cookies->new);

	my %form_data;
	for my $k (keys %$fields) {
		die "Undefined key $k" if !defined $fields->{$k};

		my $v = $fields->{$k};
		Encode::from_to($v, "utf-8", "cp1251");
		$v =~ s/(\r\n|\n)/\r\n/g;
		$form_data{$k} = $v;
	}

	# Формируем тело вручную, чтобы избежать перекодировки
	my $body = join '&', map { uri_escape($_) . '=' . uri_escape($form_data{$_}) } keys %form_data;

	my $req = HTTP::Request->new(POST => 'https://patches.kibab.com/patches/addpatch.php5');
	$req->header('Content-Type' => 'application/x-www-form-urlencoded; charset=windows-1251');
	$req->header('Cookie' => "login=$KIBAB_LOGIN; password=$KIBAB_PASSWORD");
	$req->content($body);

	my $response = $ua->request($req);

	my $code = $response->code;
	die("savePatchData(".$fields->{id}."): error: ".$response->status_line."\nbody: ".$response->decoded_content)
		if $code != 200 && $code != 302;
}

sub getPatchData {
	my ($id) = @_;

	my $ua = LWP::UserAgent->new;
	$ua->cookie_jar({});

	my $response = $ua->get(
		'https://patches.kibab.com/patches/addpatch.php5?id='.$id.'&action=edit_patch',
		Cookie => "login=$KIBAB_LOGIN; password=$KIBAB_PASSWORD"
	);

	die("getPatchData($id): error: ".$response->status_line) unless $response->is_success;

	my $doc = HTML5::DOM->new->parse($response->decoded_content);
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

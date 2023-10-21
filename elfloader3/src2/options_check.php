<?php
$config = simplexml_load_file("ELFLoader.ewp");

$common = [];
$blacklist = ['General->ExePath', 'General->ObjPath', 'General->ListPath', 'General->OGLastSavedByProductVersion', 'XLINK->XclFile'];

foreach ($config->configuration as $c) {
	foreach ($c->settings as $s) {
		foreach ($s->data as $sd) {
			foreach ($sd->option as $o) {
				$o_name = $s->name."->".$o->name;
				$o_value = (string) $o->state;
				
				if (!in_array($o_name, $blacklist)) {
					$common[$o_name] = $o_value;
				}
			}
		}
	}
}

$targets = [];
foreach ($config->configuration as $c) {
	foreach ($c->settings as $s) {
		foreach ($s->data as $sd) {
			foreach ($sd->option as $o) {
				$o_name = $s->name."->".$o->name;
				$o_value = (string) $o->state;
				
				if (!in_array($o_name, $blacklist))
					$targets[(string) $c->name][$o_name] = $o_value;
			}
		}
	}
}

foreach ($targets as $target => $options) {
	echo "$target\n";
	foreach ($common as $common_k => $common_v) {
		if (isset($options[$common_k])) {
			if ($common_v != $options[$common_k]) {
				echo "  ".$common_k.": ".$options[$common_k]." != ".$common_v."\n";
			}
		} else {
			echo "  ".$common_k.": is undef\n";
		}
	}
	
	foreach ($options as $k => $v) {
		if (!isset($common[$k])) {
			echo "  ".$k.": is unk\n";
		}
	}
	
}

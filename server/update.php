<?php
/*
    Copyright (c) 2011 Stitch Works Software
    Brian C. Milco <brian@stitchworkssoftware.com>

*/
    require_once("update-functions.php");
    require_once("license-functions.php"); // for is_valid_serial_number();

    $software = mysql_real_escape_string($_GET['sws_software']);
    $version = mysql_real_escape_string($_GET['sws_version']);
    $os = mysql_real_escape_string($_GET['sws_os']);
    $sn = mysql_real_escape_string($_GET['sws_sn']);

    $clean = array();

    if(isset($software) && is_valid_software($software))
        $clean['software'] = $software;
    else
        $clean['software'] = '';

    if(isset($version) && is_valid_version($version))
        $clean['version'] = $version;
    else
        $clean['version'] = '';

    if(isset($os) && is_valid_os($os))
        $clean['os'] = $os;
    else
        $clean['os'] = '';

    if(isset($sn) && is_valid_serial_number($sn))
        $clean['sn'] = $sn;
    else
        $clean['sn'] = '';

    if(empty($clean['software']) && (empty($clean['version']) && empty($clean['os']))) {
        print "";
        return;
    }

    $ip=mysql_real_escape_string($_SERVER['REMOTE_ADDR']);

    log_update_check($ip, $clean['software'], $clean['version'], $clean['os'], $clean['sn']);

    $server_version = current_version($software);
    $release_url = array_pop($server_version);

    $user_version = parse_version($clean['version']);
    $newer_ver = newer_version($user_version, $server_version);

    if($newer_ver == $server_version)
        $has_newer_version = true;
    else
        $has_newer_version = false;

    if($has_newer_version) {
        $version_string = $server_version[0].'.'.$server_version[1].'.'.$server_version[2];
        $url = generate_download_url($clean['software'], $version_string, $clean['os']);
        print $url."::".$release_url;
    } else {
        print '';
    }
?>

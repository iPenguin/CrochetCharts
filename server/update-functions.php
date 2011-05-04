<?php
/*
    Copyright (c) 2011 Stitch Works Software
    Brian C. Milco <brian@stitchworkssoftware.com>

*/
require_once('connection.php');

/**
 * generate_download_url should return a string in the following form:
 * http://stitchworkssoftware.com/downloads/crochet/crochet-installer-1.0.0.exe
 * @param $software should be one of the following: crochet
 * @param $version should be passed in as a string. ie: "1.0.0"
 * @param $os should be one of the following: windows, osx, linux
 */
function generate_download_url($software, $version, $os)
{
    if(empty($software) || (empty($version) || empty($os)))
        return '';

    if($_SERVER['SERVER_ADDR'] == "192.168.0.125")
        $url = 'http://192.168.0.125/~brian/downloads/';
    else
        $url  = 'http://stitchworkssoftware.com/downloads/';
    
    $url .= strtolower($software).'/'.strtolower($software).'-installer';
    $url .= '-'.$version;

    switch ($os) {
        case 'windows':
            $url .= '.exe';
            break;
        case 'osx':
            $url .= '.dmg';
            break;
        case 'linux':
            $url .= '.deb';
            break;
        default:
            $url .= '.exe';
            break;
    }

    return $url;
}

function is_valid_software($software)
{
    $software = strtolower($software);
    switch($software) {
        case 'crochet':
        case 'crochet charts':
            return true;
        default:
            return false;
    }

    return false;
}

function is_valid_version($version)
{
    $array = explode(".", $version);
    $rev   = explode("-", $array[2]);

    $major = $array[0];
    $minor = $array[1];
    $patch = array_shift($rev);

    $first_pass = true;

    foreach($rev as $value) {
        if(!empty($value)) {
            if(!$first_pass) $ginfo .= "-";
            $ginfo .= $value;
            $first_pass = false;
        }
    }

    return true;
}

function is_valid_os($os)
{
    $os = strtolower($os);
    switch ($os) {
        case 'windows':
        case 'osx':
        case 'linux':
            return true;
        default:
            return false;
    }

    return false;
}

function current_version($software)
{
    $r = mysql_query("SELECT MAX(id) as id FROM `versions` WHERE software='".$software."';");
    $row = mysql_fetch_row($r);
    $r = mysql_query("SELECT major, minor, patch, release_url FROM `versions` WHERE id=".$row[0].";");

    $row = mysql_fetch_row($r);

    return $row;
}

function parse_version($version)
{
    $array = explode(".", $version);
    $rev   = explode("-", $array[2]); //deal with any misc alpha/beta builds.
    
    $array[2] = array_shift($rev);

    return $array;
}

/**
 * newer_version compairs 2 version numbers.
 * @return which ever version is 'larger', if the
 * two version are the same returns an empty string.
 */
function newer_version($a, $b)
{
    for($i = 0; $i < 3; $i++) {
        if($a[$i] != $b[$i])
            return ($a[$i] > $b[$i]) ? $a : $b;
    }

    return '';
}

function log_update_check($ip, $software, $version, $os, $sn)
{
    //TODO: create a logging table and insert records of the update checks.
    //TODO: use these logs to check for possible misuse of serial numbers.
}

?>

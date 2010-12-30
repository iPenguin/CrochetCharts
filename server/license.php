<?php
/*
    (c) Stitch Works Software
    Brian C. Milco <brian@stitchworkssoftware.com>

*/
require_once("license-functions.php");

$sn = mysql_real_escape_string($_GET['sws_sn']);
$email = mysql_real_escape_string($_GET['sws_email']);
$fname = mysql_real_escape_string($_GET['sws_fname']);
$lname = mysql_real_escape_string($_GET['sws_lname']);

$clean = array();

if(isset($sn) && is_valid_serial_number($sn))
    $clean['sn'] = $sn;
else
    $clean['sn'] = '';

if(isset($email) && is_valid_email($email))
    $clean['email'] = $email;
else
    $clean['email'] = '';

if(isset($fname)) //TODO: check if the variable is a valid string w/o sql injection
    $clean['fname'] = $fname;
else
    $clean['fname'] = '';

if(isset($lname)) //TODO: check if the variable is a valid string w/o sql injection
    $clean['lname'] = $lname;
else
    $clean['lname'] = '';

if(empty($clean['sn']) && empty($clean['email'])) {
    print "";
    return;
}

$ip=mysql_real_escape_string($_SERVER['REMOTE_ADDR']);
$license = generate_license($clean['sn'], $clean['email']);

$r = mysql_query("INSERT INTO `stitchw1_licenses`.`licenses` " .
    "(`serial_number`, `email`, `first_name`, `last_name`, `ip_address`, `license`) VALUES (" .
    "'".$clean['sn']."','" . $clean['email'] ."','" . $clean['fname'] . "','" . $clean['lname'] .
    "','".$ip."','".$license."');");

    print $license;

?>

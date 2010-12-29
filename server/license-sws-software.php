<?php
/*
    (c) Stitch Works Software
    Brian C. Milco <brian@stitchworkssoftware.com>

*/
require_once("license-functions.php");

$sn = $_GET['sws_serial_number'];
$email = $_GET['sws_email'];
$fname = $_GET['sws_fname'];
$lname = $_GET['sws_lname'];

if(!isset($sn))
    return;
if(!isset($email))
    return;
if(!isset($fname))
    return;
if(!isset($lname))
    return;

if(is_valid($sn)) {

    $r = mysql_query("INSERT INTO `stitchw1_licenses`.`licenses` (serial_number, email, first_name, last_name) VALUES (".
    "\"".$sn."\",\"" . $email ."\",\"" . $fname . "\",\"" . $lname . "\");");

}

?>
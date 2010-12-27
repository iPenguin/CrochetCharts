<?php
$server="localhost";
$database="stitchw1_licenses";
$username="stitchw1_user";
$password="swsPasswd+2010";
$port="3306";

$link = mysql_connect($server, $username, $password);
if (!$link) {
  die('Could not connect: ' . mysql_error());
}

$db_selected = mysql_select_db($database, $link);

if (!$db_selected) {
    die ('Can\'t use foo : ' . mysql_error());
}

?>
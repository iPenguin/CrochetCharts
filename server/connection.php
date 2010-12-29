<?php
/*
    (c) Stitch Works Software 2010
    Brian C. Milco <brian@stitchworkssoftware.com>

*/

$server="localhost";
$database="stitchw1_licenses";
$username="stitchw1_user";
$password="swsPasswd+2010";
$port="3306";

/*
    return the last auto increment value for $table using $link.
*/
function table_last_auto_increment($table, $link)
{
    $r = mysql_query("SHOW TABLE STATUS LIKE '".$table."' ");
    $row = mysql_fetch_array($r);
    $auto_increment = $row['Auto_increment'];
    mysql_free_result($r);
    return $auto_increment;
}

$link = mysql_connect($server, $username, $password);
if (!$link) {
  die('Could not connect: ' . mysql_error());
}

$db_selected = mysql_select_db($database, $link);

if (!$db_selected) {
    die ('Can\'t use foo : ' . mysql_error());
}

?>
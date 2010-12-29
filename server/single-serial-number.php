<?php
require_once("license-functions.php");

/*
    (c) Stitch Works Software 2010
    Brian C. Milco <brian@stitchworkssoftware.com>

    This is an application that generates 
    a signle serial number at checkout time.
*/

$sequence = table_last_auto_increment("serialnumbers", $link);
echo generate_serial_number($sequence, true);


?>
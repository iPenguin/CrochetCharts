<!DOCTYPE HTML PUBLIC "-//W3C//DTD HTML 4.01 Transitional//EN" "http://www.w3.org/TR/html4/loose.dtd">
<?php
/*

    (c) Stitch Works Software 2010
    Brian C. Milco <brian@stitchworkssoftware.com>

This form makes it possible to generate
valid serial numbers for my software.

TODO: add a drop down to select the software
to generate the serial number for.
*/

require_once("license-functions.php");

$genSn = false;
if(isset($_POST['sn_count'])) {
    $count = $_POST['sn_count'];
    $genSn = true;
} else {
    $count = 1;
}

$commit = false;

if(isset($_POST['sn_commit']) && $_POST['sn_commit'] == "Commit") {
    $checked = "checked";
    $commit = true;
} else {
    $checked = "";
}

echo "<html>\n";
echo "<head><title>Serial Numbers</title></head>\n";
echo "<body>\n";

$nextSerialNumber = table_last_auto_increment("serialnumbers", $link);

$nextSequence = $nextSerialNumber;
if($commit)
    $nextSequence += $count;

echo "Next Sequence #: " . $nextSequence . "<br />";
echo "<form method=\"post\" action=\"" . $_SERVER['PHP_SELF'] . "\">\n";
echo "Count: <input type=\"text\" size=\"6\" maxlength=\"6\" name=\"sn_count\" value=\"".$count."\"><br />\n";
echo "Comments: <input type=\"text\: size=\"50\" name=\"sn_comment\"><br />\n";
echo "<input type=\"checkbox\" name=\"sn_commit\" value=\"Commit\" ".$checked." />Commit Records<br />\n";
echo "<input type=\"submit\" value=\"Submit\">";
echo "</form>\n";

if($genSn) {
    echo "<pre>\n";

    $total = $nextSerialNumber + $count;

    for($i = $nextSerialNumber; $i < $total; ++$i)
        echo generate_serial_number($i, $commit) . "\n";

    echo "</pre>\n";
}

echo "</body>\n";
echo "</html>\n";
?>

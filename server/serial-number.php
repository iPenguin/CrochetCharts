<!DOCTYPE HTML PUBLIC "-//W3C//DTD HTML 4.01 Transitional//EN" "http://www.w3.org/TR/html4/loose.dtd">
<?php
/*******************************************\

Copyright Brian C. Milco 2010
email: bcmilco@gmail.com

\*******************************************/
require_once("connection.php");

function generate_serial_number($sequence, $commit = false)
{
    if(is_null($sequence))
        return "";

    $seed = "BCM Software";
    $data = $seed . $sequence;

    $hash = sha1($data);

    $beginning = substr($hash, 0, 2);
    
    $end = substr($hash, strlen($hash) - 2, 2);

    $first = substr($beginning, 0, 1);
    $second = substr($beginning, 1, 1);
    $third = substr($end, 0, 1);
    $fourth = substr($end, 1, 1);

    $hexdigits[0] = hexdec($first);
    $hexdigits[1] = hexdec($second);
    $hexdigits[2] = hexdec($third);
    $hexdigits[3] = hexdec($fourth);

    $digits = '';

    foreach($hexdigits as $digit) {
        $temp = $digit / 10;
        if($temp < 1)
            $digits .= $digit;
        else if($temp >= 1)
            $digits .= $digit - 10;

    }

    $seqNum = str_pad($sequence, 7, "0", STR_PAD_LEFT);

    $serial_number = substr($seqNum, 0, 4) . "-" . substr($seqNum, 4, 3) . "-" . $digits;

    if(isset($_POST['sn_comment']))
        $comment = $_POST['sn_comment'];
    else {
        $comment = '';
    }
    

    if($commit) {
        mysql_query("INSERT INTO `serialnumbers` (serial_number, comments) VALUES (\"".$serial_number."\",\"".$comment."\");");
    }

    return $serial_number;

}

$currentNum = table_last_auto_increment("serialnumbers", $link);

$genSn = false;
if(isset($_POST['sn_count'])) {
    $count = $_POST['sn_count'];
    $genSn = true;
} else {
    $count = 1;
}

if($_POST['sn_commit'] == "Commit") {
    $checked = "checked";
} else {
    $checked = "";
}


echo "<html>\n";
echo "<head><title>Serial Numbers</title></head>\n";
echo "<body>\n";
echo "Current Sequence #: " . $currentNum . "<br />";
echo "<form method=\"post\" action=\"" . $_SERVER['PHP_SELF'] . "\">\n";
echo "Count: <input type=\"text\" size=\"6\" maxlength=\"6\" name=\"sn_count\" value=\"".$count."\"><br />\n";
echo "Comments: <input type=\"text\: size=\"50\" name=\"sn_comment\"><br />\n";
echo "<input type=\"checkbox\" name=\"sn_commit\" value=\"Commit\" ".$checked." />Commit Records<br />\n";
echo "<input type=\"submit\" value=\"Submit\">";
echo "</form>\n";

if($genSn) {
echo "<pre>\n";
    $total = $currentNum + $count;

    if($checked == "checked") {
        $commit = true;
    } else {
        $commit = false;
    }
    print $currentNum . " " . $count . " " . $total . "\n";
    for($i = $currentNum; $i < $total; ++$i)
        echo generate_serial_number($i, $commit) . "\n";

}
echo "</pre>\n";
echo "</body>\n";
echo "</html>\n";
?>

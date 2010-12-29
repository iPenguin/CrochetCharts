<?php
/*
    (c) Stitch Works Software 2010
    Brian C. Milco <brian@stitchworkssoftware.com>

*/
require_once("connection.php");

/*
    generate_serial_number(int sequence, bool commit)
    this function generates a serial number for a number
    $sequence. If commit is true it saves the serial number
    in the database.
*/
function generate_serial_number($sequence, $commit = false)
{
    if(is_null($sequence)) {
        return "";
    }

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

/*
    check that $serial_number is a valid serial number.
*/
function is_valid($serial_number)
{
    $length = strlen($serial_number);

    if($length > 13)
        return false;
    if($length < 13)
        return false;

    $sequence = substr($serial_number, 0, 8);
    $sequence = substr_replace($sequence, "", 4, 1);
    $num = (int)$sequence;

    $test = generate_serial_number($num);
    if($serial_number == $test)
        return true;
    else
        return false;
}

?>
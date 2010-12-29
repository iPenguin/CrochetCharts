<?php
/*
    (c) Stitch Works Software 2010
    Brian C. Milco <brian@stitchworkssoftware.com>

*/
require_once("connection.php");

function license_gen_values($valid_sn = false, $valid_email = false)
{
    $data = array();
    if(!$valid_sn && !$valid_email) {                //serial number generator values
        $data['seed'] = "BCM Software"; 
    } elseif($valid_sn) {                            //full license values
        $data['license_type'] = "L1";
        $data['seed'] = ";;Stitch Works Software - Full License;;";
    } elseif(!$valid_sn && $valid_email) {           //trial license values
        $data['license_type'] = "T1";
        $data['seed'] = ";;Stitch Works Software - Trial License;;";
    }

    return $data;
}

/**
    generate_serial_number(int @param sequence, bool @param commit)
    this function generates a serial number for a number
    $sequence. If commit is true it saves the serial number
    in the database.
    @return $serial_number
*/
function generate_serial_number($sequence, $commit = false)
{
    if(is_null($sequence)) {
        return "";
    }

    $info = license_gen_values(false, false);
    $data = $info['seed'] . $sequence;
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
        $comment = mysql_real_escape_string($_POST['sn_comment']);
    else {
        $comment = '';
    }

    if($commit) {
        mysql_query("INSERT INTO `serialnumbers` (serial_number, comments) VALUES (\"".$serial_number."\",\"".$comment."\");");
    }

    return $serial_number;
}

/**
    check that $serial_number is a valid serial number.
*/
function is_valid_serial_number($serial_number)
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

function generate_license($serial_number, $email)
{
    $valid_sn = is_valid_serial_number($serial_number);
    $valid_email = is_valid_email($email);

    if(!valid_sn && !valid_email)
        return '';

    $info = license_gen_values($valid_sn, $valid_email);
    $license_type = $info['license_type'];
    $seed = $info['seed'];
       
    $data = $seed . $serial_number . $license_type;
    $hash = sha1($data);
    $beginning = substr($hash, 0, 4);
    $middle = substr($hash, 18, 4);
    $end = substr($hash, strlen($hash) - 4, 4);

    $license = $beginning."-".$middle."-".$end."-".$license_type;

    $license = strtoupper($license);

    return $license;
}

function is_valid_email($email)
{
    return true; //TODO: RegEx check for a valid email.
}

function is_valid_license_type($license_type)
{
    switch($license_type) {
        case "L1": // License for version 1
        case "T1": // Trial for version 1
            return true;
            break;
        default:
            return false;
    }

    return false;
}
?>
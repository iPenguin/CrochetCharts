<?php
/*************************************************\
| Copyright (c) 2011 Stitch Works Software        |
| Brian C. Milco <brian@stitchworkssoftware.com>  |
\*************************************************/

require_once('connection.php');

function get_current_version($software)
{
    $r = mysql_query("SELECT MAX(id) as id FROM `versions` WHERE software='".strtolower($software)."';");
    $row = mysql_fetch_row($r);

    $r = mysql_query("SELECT major, minor, patch, release_url, timestamp FROM `versions` WHERE id=".$row[0].";");
    $row = mysql_fetch_row($r);

    return $row;

}
    //TODO: add some id's and class' so it can be CSS-ified.

    $product[] = "Crochet";

    $html = "<table>\n";
#    $html .= "<tr>\n";
#    $html .= "<th align=\"left\">Software</th>\n";
#    $html .= "<th align=\"left\">Version</th>\n";
#    $html .= "<th align=\"left\">Released</th>\n";
#    $html .= "</tr>\n";

    foreach($product as $software) {
        $info = get_current_version($software);
        
        $html .= "<tr>\n";
        $html .= "<td>".$software."</td>\n";

        $version = $info[0].".".$info[1].".".$info[2];

        if(empty($info) || $version == "0.0.0") {
            $html .= "<td colspan = 2>Coming Soon</td>\n";
        
        } else {
            $html .= "<td><a href=\"".$info[3]."\">".$version."</a></td>\n";
            $html .= "<td>".date("j M Y", strtotime($info[4]))."</td>\n";
        }
        
        $html .= "</tr>\n";
    }

    $html .= "</table>\n";

    print $html;

?>

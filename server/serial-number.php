<?php
include("connection.php");

print "<form method='post' action=\"" . $PHP_SELF . "\">";
print "First Name:<input type=\"text\" size=\"12\" maxlength=\"12\" name=\"Fname\">:<br />";
print "Last Name:<input type='text' size='12' maxlength='36' name='Lname'>:<br />";
print "";
print "</form>";

?>
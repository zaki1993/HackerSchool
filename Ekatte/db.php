<?php

 $dbhost = 'localhost';
   $dbuser = 'streetzaki';
   $dbpass = '14eiuqhwdyeuq';
   $dbname = 'excelSql';
   $conn = mysql_connect($dbhost, $dbuser, $dbpass);
   if(! $conn )
   {
     die('Could not connect: ' . mysql_error());
  }

$db_selected = mysql_select_db('excelSql', $conn);
if (!$db_selected) {
    die ('Can\'t use database : ' . mysql_error());
}


<?php
   $dbhost = 'localhost';
   $dbuser = 'streetzaki';
   $dbpass = '14eiuqhwdyeuq';
   $conn = mysql_connect($dbhost, $dbuser, $dbpass);
   if(! $conn )
   {
     die('Could not connect: ' . mysql_error());
  }

global $wpdb;
$result = $wpdb->get_results( "SELECT * FROM wp_coordinates" );

foreach($result as $res){
	 $i = $res->coordinates_id;
}		
		$x = 0;		
		$url = "http://www.seismi.org/api/eqs/2011/05?min_magnitude=2";
		$arr = json_decode(file_get_contents($url));
if($arr){
	foreach($arr->earthquakes as $row){
	$wpdb->insert('wp_coordinates',
		array(
			'coordinates_id' => $i+1,
			'coordinates_lat' => $arr->earthquakes[$x]->lat,
			'coordinates_lng' => $arr->earthquakes[$x]->lon,
			'coordinates_address' => $arr->earthquakes[$x]->region,
			'coordinates_date' => $arr->earthquakes[$x]->timedate,
			'coordinates_magnitude' => $arr->earthquakes[$x]->magnitude
		),
		array('%d','%g','%g','%s','%s','%g'));
		$i++;
		$x++;
	}

}

   mysql_close($conn);
?>

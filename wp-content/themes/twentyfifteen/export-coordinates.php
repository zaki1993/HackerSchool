<style type="text/css">
#filter-container-timedate,#filter-container-magnitude{
	margin: 0px;
	padding: 0px;
	display:inline-block;
}
#label-date{
display:inline;
}
#timedate-input-from,#timedate-input-to,#magnitude-input-from,#magnitude-input-to{
display:inline;
width: 350px;
}

</style>

<div id="filter-container-timedate">
<form action="" method="post">
<span>Timedate: </span>
<input name="input_date_from" id="timedate-input-from" type="date" placeholder="From"></input>
<input name="input_date_to" id="timedate-input-to" type="date" placeholder="To"></input>
<button name="timedate-check" id="timedate-check" name="timedate" type="submit">Timedate</button>
<span>Magnitude: </span>
<input name="input_magnitude_from" id="magnitude-input-from" type="text" placeholder="From"></input>
<input name="input_magnitude_to" id="magnitude-input-to" type="text" placeholder="To"></input>
<button id="magnitude-check" name="magnitude" type="submit">Magnitude</button>
</form>
</div>

<?php
   $dbhost = 'localhost';
   $dbuser = 'streetzaki';
   $dbpass = '14eiuqhwdyeuq';
   $conn = mysql_connect($dbhost, $dbuser, $dbpass);
   if(! $conn )
   {
     die('Could not connect: ' . mysql_error());
  }
$counter = 0;
if($_POST["input_date_from"] && $_POST["input_date_to"]){
$from = $_POST["input_date_from"];
$to = $_POST["input_date_to"];
$mylink = $wpdb->get_results( "SELECT * FROM wp_coordinates WHERE coordinates_date>='$from' and coordinates_date <='$to'");
}
if($_POST["input_magnitude_from"] && $_POST["input_magnitude_to"]){
$from = $_POST["input_magnitude_from"];
$to = $_POST["input_magnitude_to"];
$mylink = $wpdb->get_results( "SELECT * FROM wp_coordinates WHERE coordinates_magnitude>='$from' and coordinates_magnitude <='$to'");
}
if($mylink){
 echo '<table>';
foreach($mylink as $date){
$counter++;
echo '<script>
 var marker = new google.maps.Marker({ 
    position: new google.maps.LatLng('.$date->coordinates_lat.', '.$date->coordinates_lng.' ), 
    map: map, 
    title:"'.$date->coordinates_address.'"
  });
</script>';
     
      echo '<tr>';
      echo '<td>' . $date->coordinates_lat.'</td>';
      echo '<td>' . $date->coordinates_lng.'</td>';
      echo '<td>' . $date->coordinates_address.'</td>';
      echo '<td>' . $date->coordinates_date.'</td>';
      echo '<td>' . $date->coordinates_magnitude.'</td>';
      echo '</tr>';

}
      echo '</table>';
}
echo "The number of markers is: ";
echo $counter;
/**
$result = $wpdb->get_results( "SELECT * FROM wp_coordinates" );
 echo '<table>';
    foreach ( $result as $print )   {
      echo '<tr>';
      echo '<td>' . $print->coordinates_lat.'</td>';
      echo '<td>' . $print->coordinates_lng.'</td>';
      echo '<td>' . $print->coordinates_address.'</td>';
      echo '<td>' . $print->coordinates_date.'</td>';
      echo '<td>' . $print->coordinates_magnitude.'</td>';
      echo '</tr>';
      echo ' <script> 
  var marker = new google.maps.Marker({ 
    position: new google.maps.LatLng('.$print->coordinates_lat.', '.$print->coordinates_lng.' ), 
    map: map, 
    title:"'.$print->coordinates_address.'"
  });
</script>';
  }

 echo '</table>';
*/
mysql_close($conn);
?>  


<style type="text/css">		
		#map-canvas {			
			width:    100%;		
			height:   500px;
			}			
	</style>		
<div id="map-canvas">
</div>
<!-- #map-canvas -->		



<script type="text/javascript" src="http://maps.googleapis.com/maps/api/js?key=AIzaSyCUMxd0msttVY6Bz2dCxu1d6Jmqp5GtaAo"></script>		

<script type="text/javascript">				google.maps.event.addDomListener( window, 'load', gmaps_results_initialize );
/**
 * Renders a Google Maps centered on Atlanta, Georgia. This is done by using
 * the Latitude and Longitude for the city.
 *
 * Getting the coordinates of a city can easily be done using the tool availabled
 * at: http://www.latlong.net
 *
 * @since    1.0.0
 */

var map = new google.maps.Map( document.getElementById( 'map-canvas' ), {

		zoom:           2,
		center:         new google.maps.LatLng( 40, 3 ),

	});
//initializing the map
function gmaps_results_initialize() {
	if ( null === document.getElementById( 'map-canvas' ) ) {
		return;
	}
}				

</script>

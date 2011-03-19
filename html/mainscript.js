var vis = [];
var p = [];
var DX = [];

function _gel(x) {return document.getElementById(x)}

function doAJAX(target) {

	xmlhttp = (window.XMLHttpRequest)?new XMLHttpRequest():(window.ActiveXObject)?new ActiveXObject("Microsoft.XMLHTTP"):null;
	
	if(xmlhttp == null) {// AJAX not supported
		alert('AJAX not Supported');
		return;
	}

	xmlhttp.open("GET", (target) ,false);
	xmlhttp.send(null);
	
	return xmlhttp.responseText;
}

function getData( plotType, dataLoc, width, height) { // minimum args = 4


	this.w = width;
	this.h = height;

	this.data = JSON.parse( doAJAX( dataLoc ) );

	switch( plotType ) {

	case 'scatterplot':
		// scatterplot requires 3 more arguments: the names of the x,y,z fields
		var vx = arguments[4]||'x', vy = arguments[5]||'y', vz = arguments[6]||'z';
		this.xval 	= new Function("d", "return this.root.x(d." + vx + ")");
		this.yval 	= new Function("d", "return this.root.y(d." + vy + ")");
		this.colour = new Function("d", "return this.root.c(d." + vz + ")");
		this.size 	= new Function("d", "return d." + vz);
		this.title 	= new Function("d", "return d." + vz + ".toFixed(1)");
		break;

	case 'focus+zoom':
		//focus+zoom requires 3 more arguments: height of small graph, and names of x,y fields
		this.h2 = arguments[4];

		var vx = arguments[5]||'x', vy = arguments[6]||'y';
		this.x		= new Function("d", "return d." + vx);
		this.y		= new Function("d", "return d." + vy);
		this.xval	= new Function("d", "return this.root.x(d." + vx + ")");
		this.yval	= new Function("d", "return this.root.y(d." + vy + ")");
		this.fx		= new Function("d", "return this.root.fx(d." + vx + ")");
		this.fy		= new Function("d", "return this.root.fy(d." + vy + ")");
		break;

	default:
		alert('What the heck is a "' + plotType + '" ?!');
	}

	return this;
}

function doDataUpdate( command ) {

	var response = JSON.parse( doAJAX( command ) );
	
	var u = response.update;
	
	for( i=0; i<u.length; i++) {
	
		vis[ u[i] ].tdata.data( response[ u[i] ] );
		vis[ u[i] ].s = false; //removes earlier selection
		vis[ u[i] ].render();
	}
}

function doSelection() {
	
}
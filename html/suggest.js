function autoCompleteDB() {this.aNames = new Array();}
 
autoCompleteDB.prototype.assignArray = function(aList) {this.aNames = aList;};
 
autoCompleteDB.prototype.getMatches = function(str,aList,maxSize) {

	var ctr = 0;
	for(var i in this.aNames) {
		if( this.aNames[i].toLowerCase().indexOf( str.toLowerCase() ) == 0 ) { // look for case insensitive matches
			aList.push( this.aNames[i] );
			ctr++;
		}
		if( ctr == (maxSize-1) ) //limit # of matches to maxSize
			break;
	}
};
 
function autoComplete( aNames, oText, oDiv, maxSize ) {
	this.oText = oText;
	this.oDiv = oDiv;
	this.maxSize = maxSize;
	this.cur = -1;

	this.db = new autoCompleteDB();
	this.db.assignArray(aNames);

	oText.onkeyup = this.keyUp;
	oText.onkeydown = this.keyDown;
	oText.autoComplete = this;
	oText.onblur = this.hideSuggest;
}
 
autoComplete.prototype.hideSuggest = function( event ) {

	if(typeof selectionComplete == 'function')
		selectionComplete(this);

	this.autoComplete.oDiv.style.visibility = 'hidden';

	if(event.type == 'blur')
		event.stopPropagation(); // prevent event from occurring uselessly
};
 
autoComplete.prototype.selectText = function( iStart, iEnd ) {

	if( this.oText.createTextRange ) { // For IE
		var oRange = this.oText.createTextRange();
		oRange.moveStart('character',iStart);
		oRange.moveEnd('character',iEnd-this.oText.value.length);
		oRange.select();
	}
	else if( this.oText.setSelectionRange )  { // For Mozilla
		this.oText.setSelectionRange(iStart,iEnd);
	}
	
	this.oText.focus();
};
 
autoComplete.prototype.textComplete = function(sFirstMatch) {
	if( this.oText.createTextRange || this.oText.setSelectionRange ) {
		var iStart = this.oText.value.length;
		this.oText.value = sFirstMatch;
		this.selectText(iStart,sFirstMatch.length);
	}
};
 
autoComplete.prototype.keyDown = function( oEvent ) {
	oEvent = window.event || oEvent;
	iKeyCode = oEvent.keyCode; // var'd elsewhere. DO NOT var'ify

	switch( iKeyCode ) {
	
	case 38: //up arrow
		this.autoComplete.moveUp();
		break;
		
	case 40: //down arrow
		this.autoComplete.moveDown();
		break;
		
	case 13: //return key
		this.blur();
		this.focus();
		break;
		
	}
};
 
autoComplete.prototype.moveDown = function() {
	if( this.oDiv.childNodes.length>0 && this.cur < (this.oDiv.childNodes.length-1) ) {
		++this.cur;
		for(var i=0; i < this.oDiv.childNodes.length; i++ ) {
			if( i == this.cur ) {
				this.oDiv.childNodes[i].className = 'over';
				this.oText.value = this.oDiv.childNodes[i].innerHTML;
			}
			else {
				this.oDiv.childNodes[i].className = '';
			}
		}
	}
};
 
autoComplete.prototype.moveUp = function() {
	if(this.oDiv.childNodes.length>0 && this.cur>0) {
		--this.cur;
		for(var i=0;i<this.oDiv.childNodes.length;i++) {
			if(i==this.cur) {
				this.oDiv.childNodes[i].className = 'over';
				this.oText.value = this.oDiv.childNodes[i].innerHTML;
			}
			else {
				this.oDiv.childNodes[i].className = '';
			}
		}
	}
};
 
autoComplete.prototype.keyUp = function( oEvent ) {

	oEvent = oEvent || window.event;
	var iKeyCode = oEvent.keyCode;
	
	if( iKeyCode == 8 || iKeyCode == 46 ) {
		this.autoComplete.onTextChange(false); // without autocomplete
	}
	else if (iKeyCode < 32 || (iKeyCode >= 33 && iKeyCode <= 46) || (iKeyCode >= 112 && iKeyCode <= 123))  {
		//ignore
	} 
	else {
		this.autoComplete.onTextChange(true); // with autocomplete
	}
};

// calculate dropdown location
autoComplete.prototype.positionSuggest = function() {
	var oNode = this.oText;
	var x=0,y=oNode.offsetHeight;

	while( oNode.offsetParent && oNode.offsetParent.tagName.toUpperCase() != 'BODY' ) {
		x += oNode.offsetLeft;
		y += oNode.offsetTop;
		oNode=oNode.offsetParent;
	}

	x += oNode.offsetLeft;
	y += oNode.offsetTop;

	this.oDiv.style.top = y + 'px';
	this.oDiv.style.left = x + 'px';
}
 
autoComplete.prototype.onTextChange = function( bTextComplete ) {

	var txt = this.oText.value;
	var oThis = this;
	this.cur = -1;

	if( txt.length > 0 ) {
	
		while(this.oDiv.hasChildNodes())
			this.oDiv.removeChild(this.oDiv.firstChild);

		var aStr = new Array();
		this.db.getMatches(txt,aStr,this.maxSize);
		
		if(!aStr.length) { this.hideSuggest; return }
		
		if(bTextComplete)
			this.textComplete(aStr[0]);
		
		this.positionSuggest();

		for( i in aStr ) {
		
			var oNew = document.createElement('div');
			this.oDiv.appendChild(oNew);
			
			oNew.onmouseover = oNew.onmouseout = oNew.onmousedown = function( oEvent ) {
			
				oEvent = window.event || oEvent;
				oSrcDiv = oEvent.target || oEvent.srcElement;

				if( oEvent.type == 'mousedown' ) {
					oThis.oText.value = this.innerHTML;
				}
				else if( oEvent.type == 'mouseover' ) {
					this.className = 'over';
				}
				else if( oEvent.type == 'mouseout' ) {
					this.className = '';
				}
				else {
					this.oText.focus();
				}
			};
			oNew.innerHTML = aStr[i];
		}

		this.oDiv.style.visibility = 'visible';
	}
	else {
		this.oDiv.innerHTML = '';
		this.oDiv.style.visibility = 'hidden';
	}
};
 
function createAutoComplete( options, txt, suggest ) {
	return new autoComplete(options,txt,suggest);
}

function getParam(name)
{  
    name = name.replace(/[\[]/,"\\\[").replace(/[\]]/,"\\\]");  
    var regexS = "[\\?&]"+name+"=([^&#]*)";  
    var regex = new RegExp( regexS );  
    var results = regex.exec(window.location.href);
    if(results == null)
        return "";  
    else    
        return results[1];
}

function processOnLoadSearch()
{
	submitSearch(getParam("searchedit"));
}

function submitSearch(term)
{
	var results = index.search(term);
	document.getElementById('searchresults').innerHTML = "<h1>Search Results</h1>";
	
	for(var i = 0; i < results.length; i++)
	{
		var site = sites[parseInt(results[i].ref) + 1];
		var entry = "<a class='filelink' href='" + site.link + "'>";
		entry += site.title + "</a><br>";
		document.getElementById('searchresults').innerHTML += entry;
	}
}

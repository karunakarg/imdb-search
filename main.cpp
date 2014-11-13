/**
IMDB Movie/Tv Series Search
Copyright (c) 2014, Karunakar Gautam, <karunakar.gautam@gmail.com>.

All rights reserved.

Permission to use, copy, modify, and distribute this software for any purpose
with or without fee is hereby granted, provided that the above copyright
notice and this permission notice appear in all copies.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT OF THIRD PARTY RIGHTS. IN
NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM,
DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR
OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE
OR OTHER DEALINGS IN THE SOFTWARE.

Except as contained in this notice, the name of a copyright holder shall not
be used in advertising or otherwise to promote the sale, use or other dealings
in this Software without prior written authorization of the copyright holder.
 */

#include <iostream>
#include "rapidjson/document.h"
#include "rapidjson/writer.h"
#include "rapidjson/stringbuffer.h"
#include <stdlib.h>
#include <string>
#include <curl/curl.h>
using namespace std;
using namespace rapidjson;

std::string buffer;

size_t curl_write( void *ptr, size_t size, size_t nmemb, void *stream)
{
buffer.append((char*)ptr, size*nmemb);
return size*nmemb;
}

int main(){
	CURL *curl;
	CURLcode res;
	int start,i;
	string tmptitle,title,url;
	while(1){
		cout<<"\nEnter Title to search:";
		getline (std::cin,tmptitle); //take input
		if(tmptitle==""){ //if empty input, skip everything
			continue;
		}
		start = 0;
		//replace spaces with %2f
		for(i=0;i<tmptitle.length();i++){
			if(tmptitle[i] == ' '){
				title.append(tmptitle,start,i-start);
				title = title+"%2f";
				start = i+1;
			}
		}
		title.append(tmptitle,start,i-start);
		curl = curl_easy_init();
		url = "http://www.omdbapi.com/?t="+title+"&plot=full"; //API URL
		cout<<"\nSearching: "<<tmptitle<<"\n";
		cout<<"\nPlease wait...\n\n";
		if(curl) {
			//set url to visit
			curl_easy_setopt(curl, CURLOPT_URL, url.c_str());
			/* if URL is redirected, we tell libcurl to follow redirection */ 
			curl_easy_setopt(curl, CURLOPT_FOLLOWLOCATION, 1L);
			curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, curl_write);
			/* Perform the request, res will get the return code */ 
			res = curl_easy_perform(curl);
			/* Check for errors */ 
			if(res != CURLE_OK){
				fprintf(stderr, "curl_easy_perform() failed: %s\n",curl_easy_strerror(res));
			}
			try{
				//fwrite( buffer.c_str(), buffer.length(), sizeof(char), stdout); //uncomment to see raw JSON response
				if(buffer.length()<100){ //if response is too short, it's probably an error. DO NOT PARSE!
					cout<<"ERROR: Title Not Found! Please check the spelling or try another title\n";
				}
				else{
					// Parse the response and fill relevant variables
					const char* json = buffer.c_str();
					Document d;
					d.Parse(json);
					Value& name = d["Title"];
					Value& year = d["Year"];
					Value& genre = d["Genre"];
					Value& plot = d["Plot"];
					Value& runtime = d["Runtime"];
					Value& imdbId = d["imdbID"];
					Value& rating = d["imdbRating"];
					Value& director = d["Director"];
					Value& actors = d["Actors"];
					// Time to see the results
					cout<<"Title: "<<name.GetString()<<" - "<<year.GetString()<<endl;
					cout<<"Genre: "<<genre.GetString()<<endl;
					cout<<"Rating: "<<rating.GetString()<<endl;
					cout<<"Director: "<<director.GetString()<<endl;
					cout<<"Actors: "<<actors.GetString()<<endl;
					cout<<"Runtime: "<<runtime.GetString()<<endl;
					cout<<"IMDB Link: "<<"http://www.imdb.com/title/"<<imdbId.GetString()<<"/"<<endl;
					cout<<"Plot: "<<endl<<"-----------------------------------------------------------------------------\n";
					cout<<plot.GetString()<<endl;
				}
			}
			catch(...){ // catch before code dies
				//fwrite( buffer.c_str(), buffer.length(), sizeof(char), stdout); //uncomment to see raw JSON response
				cout<<"Bad luck!! Try Again!";
			}
			/* always cleanup */
			buffer.clear();
			curl_easy_cleanup(curl);
			/* empty the title for reuse in next iteration */
			title.erase(title.begin(),title.end());
		}
	}
	
	return 0;
}

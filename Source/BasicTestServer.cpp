/*
 *  BasicTestServer.cpp
 *  VisualQuerySys
 *
 *  Created by Azza Abouzied on 1/8/11.
 *  Copyright 2011 Yale University. All rights reserved.
 *
 */


#include <stdio.h>
#include <stdlib.h>
#include <iostream>
#include <fstream>
#include <string>
#include <map>

#include <Schema.hpp>

#include <boost/cgi/fcgi.hpp>
#include <boost/shared_ptr.hpp>

int handle_request(boost::fcgi::request& r);

int main_(){
    try{
        boost::fcgi::service s;
        boost::fcgi::acceptor a(s);
        
        int status(0);
        do{
            status = a.accept(&handle_request);
        }
        while(!status);
        
        return status;
    }
    catch(boost::system::system_error const& se){
        std::cerr<< "[fcgi] System error: " << se.what() << std::endl;
        return -1;
    }
    catch(std::exception const& e){
        // Catch any other exceptions
        std::cerr<< "[fcgi] Exception: " << e.what() << std::endl;
        return -2;
    }
    catch(...){
        std::cerr<< "[fcgi] Uncaught exception!" << std::endl;
        return -3;
    }
    
    return EXIT_SUCCESS;
}

int handle_request(boost::fcgi::request& r){
    boost::system::error_code ec;
    
    //
    // Load in the request data so we can access it easily.
    //
    std::cerr << "Parse all data" << std::endl;
    r.load(boost::fcgi::parse_all);

	int i = r.get["pid"]
		if(r.get["command"] == "getControlTbl"){
			
		}
		else if(r.get["command"] == "getData"){
		
		}
    
    /*
	 if(r.get["command"] == "create"){
	 if(r.get["type"] == "sc"){
	 std::string xcol_name = r.get["x"];
	 std::string ycol_name = r.get["y"];
	 Schema s;
	 Schema::getSchema(s, "fooddb");
	 
	 std::string typex = s.getType(xcol_name);
	 std::string typey = s.getType(ycol_name);
	 
	 //Convert this to a function that reads types and returns any column
	 if(typex == typeid(double).name())
	 boost::shared_ptr<Column <double> > xcol_ptr = s.getColumn<double>(xcol_name);
	 if(typey == typeid(double).name())
	 boost::shared_ptr<Column <double> > ycol_ptr = s.getColumn<double>(ycol_name);
	 if(typey == typeid(double).name() && typey == typeid(double).name()){
	 Scatterplot<double, double>* sc = new Scatterplot();
	 sc->set(xcol_ptr, ycol_ptr);
	 }
	 }
	 }
     */
	
    boost::fcgi::response resp;
    std::cerr << "Create Response" << std::endl;
    resp << boost::fcgi::content_type("text/html");
    resp << "<html><head><title>FastCGI Echo Example</title><head><body>Request ID = " << r.id() << "<br/>Process ID = " << getpid() << "<br/><form method=post enctype=\"multipart/form-data\"><input type=text name=name value='" << r.post["name"] << "' /><br /><input type=text name=hello value='" << r.post["hello"] << "' /><br /><input type=file name=user_file /><input type=hidden name=cmd value=multipart_test /><br /><input type=submit value=submit /></form><p />";
    //format_map(resp, r, r.post, "POST Variables");
    // Print the buffer containing the POST data and the FastCGI params.
    resp<< "<pre>";
    resp << std::string(r.post_buffer().begin(), r.post_buffer().end());
    resp<< "</pre>";
    
    resp << "Response content-length == " << resp.content_length(); // the content-length (returns std::size_t)
    
    return boost::fcgi::commit(r, resp);
}




'use strict'

// C library API
const ffi = require('ffi-napi');

// Express App (Routes)
const express = require("express");
const app     = express();
const path    = require("path");
const fileUpload = require('express-fileupload');

app.use(fileUpload());
app.use(express.static(path.join(__dirname+'/uploads')));

// Minimization
const fs = require('fs');
const JavaScriptObfuscator = require('javascript-obfuscator');

// Important, pass in port as in `npm run dev 1234`, do not change
const portNum = process.argv[2];

// Send HTML at root, do not change
app.get('/',function(req,res){
  res.sendFile(path.join(__dirname+'/public/index.html'));
});

// Send Style, do not change
app.get('/style.css',function(req,res){
  //Feel free to change the contents of style.css to prettify your Web app
  res.sendFile(path.join(__dirname+'/public/style.css'));
});

// Send obfuscated JS, do not change
app.get('/index.js',function(req,res){
  fs.readFile(path.join(__dirname+'/public/index.js'), 'utf8', function(err, contents) {
    const minimizedContents = JavaScriptObfuscator.obfuscate(contents, {compact: true, controlFlowFlattening: true});
    res.contentType('application/javascript');
    res.send(minimizedContents._obfuscatedCode);
  });
});

//Respond to POST requests that upload files to uploads/ directory
app.post('/upload', function(req, res) {
  if(!req.files) {
    return res.status(400).send('No files were uploaded.');
  }

  let uploadFile = req.files.uploadFile;

  // Use the mv() method to place the file somewhere on your server
  uploadFile.mv('uploads/' + uploadFile.name, function(err) {
    if(err) {
      return res.status(500).send(err);
    }

    res.redirect('/');
  });
});

//Respond to GET requests for files in the uploads/ directory
app.get('/uploads/:name', function(req , res){
  fs.stat('uploads/' + req.params.name, function(err, stat) {
    if(err == null) {
      res.sendFile(path.join(__dirname+'/uploads/' + req.params.name));
    } else {
      console.log('Error in file downloading route: '+err);
      res.send('');
    }
  });
});

//******************** Your code goes here ********************
//C library functions
let sharedLib = ffi.Library('./libgpxparser.so', {
 "retSTR" : ["string",["string"]],
 "retRoute" : ["string",["string"]],
 "retTrack" : ["string",["string"]],
 "retFile" : ["int",["string","double","string"]],
 "addingpoint" : ["int",["string","string","double","double"]],
 //arraytoret(char * fileName,float sourceLat, float sourceLong, float destLat, float destLong, float delta){
 "routeFindPath" : ["string",["string","float","float","float","float",'float']],
 "trackFindPath" : ["string",["string","float","float","float","float",'float']],
 "changeName" : ["int",["string","string","string"]],
 "showOther" : ["string",["string","string"]],
});
////////////////////////////////////////////////////////////////////////////////////

//Sample endpoint
app.get('/endpoint1', function(req , res){
  let retStr = req.query.data1 + " " + req.query.data2;
  res.send(
    {
      somethingElse: retStr
    }
  );
});
////////////////////////////////////////////////////////////////////////////////////

app.get('/filelog1', function(req , res){
  let path = require('path');
  let fs = require('fs');
  //this is making a dir Path
  let directoryPath = path.join(__dirname, 'uploads');
  var files = fs.readdirSync(directoryPath);
  let arr = [];
  files.forEach(function (Filename) {
    if (Filename.substr(Filename.length-4, Filename.length-1)==='.gpx') {
      //parsing file
      let str = sharedLib.retSTR("uploads/"+Filename);
     str = JSON.parse(str);
     str.filename = Filename;
     arr.push(str);
    }
  });
  //console.log(arr);
  res.send(
    {
      return: arr
    }
  );
});
////////////////////////////////////////////////////////////////////////////////////

app.get('/viewpanel', function(req , res){
  //console.log('test');
  let str = sharedLib.retRoute("uploads/"+req.query.filename);
  //console.log('test:  '+str);
   str = JSON.parse(str);
   let str2 = sharedLib.retTrack("uploads/"+req.query.filename);
   str2 = JSON.parse(str2);
   //console.log(str);
  res.send(
    {
      routes: str,
      tracks: str2
    }
  )
});
////////////////////////////////////////////////////////////////////////////////////

app.get('/gpxcreater', function(req , res){
  //console.log('test');
  //
  let ret = 0;
  let value = sharedLib.retFile("uploads/"+req.query.Filename,parseFloat(req.query.Version),req.query.Creator);
  if (value == 1) {
     ret = 1;
  }else {
    ret = 0;
  }
  //console.log(ret + "test0");
  res.send(
    {
      return: ret
    }
  )
});
////////////////////////////////////////////////////////////////////////////////////
app.get('/addRoutes', function(req , res){
  //console.log("hello world");
  //console.log("the querie are " +  req.query.filename + "   " +  req.query.name + parseFloat(req.query.lat)+ "   " + parseFloat(req.query.lon));
  let value = 0;
  let num = sharedLib.addingpoint("uploads/"+req.query.filename,req.query.name,parseFloat(req.query.lat),parseFloat(req.query.lon));
  //console.log('test:  '+ num);
 if (num ==1) {
   value == 1;
 }else {
   value == 0;
 }
   //console.log(str);
  res.send(
    {
      return : num
    }
  )
});
////////////////////////////////////////////////////////////////////////////////////
app.get('/pathBetween', function(req , res){
  let path = require('path');
  let fs = require('fs');
  //this is making a dir Path
  let directoryPath = path.join(__dirname, 'uploads');
  var files = fs.readdirSync(directoryPath);
  let routeList = [];
  let trackList = [];
  files.forEach(function (Filename) {
    if (Filename.substr(Filename.length-4, Filename.length-1)=='.gpx') {
      //parsing file
      let route = sharedLib.routeFindPath("uploads/"+Filename,parseFloat(req.query.srclat),parseFloat(req.query.srclon),parseFloat(req.query.destlat),parseFloat(req.query.destlon),parseFloat(req.query.accuracy));
      route = JSON.parse(route);
      routeList = routeList.concat(route);

      let track = sharedLib.routeFindPath("uploads/"+Filename,parseFloat(req.query.srclat),parseFloat(req.query.srclon),parseFloat(req.query.destlat),parseFloat(req.query.destlon),parseFloat(req.query.accuracy));
      track = JSON.parse(track);
      trackList = trackList.concat(track);
    }
  });

  res.send(
    {
      routes : routeList,
      tracks : trackList
    }
  )
});

app.get('/rename', function(req , res){
  let retval = sharedLib.changeName('uploads/'+req.query.filename, req.query.oldName, req.query.newName);
  res.send(
    {
      value : retval
    }
  )
});

app.get('/showData', function(req , res){
  let list = sharedLib.showOther('uploads/'+req.query.filename, req.query.name);
  let retval = JSON.parse(list);
  res.send(
    {
      retData : retval
    }
  )
});

app.listen(portNum);
console.log('Running app at localhost: ' + portNum);

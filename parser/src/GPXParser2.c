#include <stdio.h>
#include <stdlib.h>
#include <libxml/parser.h>
#include <libxml/tree.h>
#include "GPXParser.h"
#include <stdbool.h>
#include <math.h>
#include <libxml/xmlschemastypes.h>
int validater(xmlDocPtr doc, char *gpxSchemaFile)
{

  xmlSchemaPtr schema = NULL;
  xmlSchemaParserCtxtPtr ctxt;
  int ret;
  // filename for both are provided as func args
  xmlLineNumbersDefault(1);
  ctxt = xmlSchemaNewParserCtxt(gpxSchemaFile);

  xmlSchemaSetParserErrors(ctxt, (xmlSchemaValidityErrorFunc)fprintf, (xmlSchemaValidityWarningFunc)fprintf, stderr);
  schema = xmlSchemaParse(ctxt);
  xmlSchemaFreeParserCtxt(ctxt);
  //xmlSchemaDump(stdout, schema); //To print schema dump

  //doc = xmlReadFile(fileName, NULL, 0);

  if (doc == NULL)
  {
    //"Could not parse file
    return 1;
  }
  else
  {
    xmlSchemaValidCtxtPtr ctxt;

    ctxt = xmlSchemaNewValidCtxt(schema);
    xmlSchemaSetValidErrors(ctxt, (xmlSchemaValidityErrorFunc)fprintf, (xmlSchemaValidityWarningFunc)fprintf, stderr);
    ret = xmlSchemaValidateDoc(ctxt, doc);
    if (ret == 0)
    {
      // meaning validation is good
      //actualDoc = createGPXdoc(fileName);
      // if return true, then say return true
      //return ret;
    }
    else if (ret > 0)
    {
      // fails to validate
      //return ret;
    }
    else
    {
      // validation generated an internal error;
      //return ret;
    }
    xmlSchemaFreeValidCtxt(ctxt);
    xmlFreeDoc(doc);
  }

  // free the resource
  if (schema != NULL)
  {
    xmlSchemaFree(schema);
  }
  //free the rest
  xmlSchemaCleanupTypes();
  xmlCleanupParser();
  xmlMemoryDump();
  return ret;
}
////////////////////////////////////////////////////////////////////////////////////////////
GPXdoc *createValidGPXdoc(char *fileName, char *gpxSchemaFile)
{
  xmlDocPtr doc;
  doc = xmlReadFile(fileName, NULL, 0);
  GPXdoc *actualDoc;
  if (fileName == NULL || gpxSchemaFile == NULL)
  {
    return NULL;
  }
  int validityCheck = validater(doc, gpxSchemaFile);

  if (validityCheck == 0)
  {
    actualDoc = createGPXdoc(fileName);
  }
  else
  {
    return NULL;
  }
  return actualDoc;
}
///////////////////////////////////////////////////////////////////////////////////////////////
void waypointMaker(List *doc, xmlNodePtr node, xmlNodePtr root_node, char *name)
{
  char lat[100], lon[100];
  xmlNodePtr info = NULL, wptnode = NULL;
  ListIterator iter = createIterator(doc);
  for (Waypoint *wpt = nextElement(&iter); wpt != NULL; wpt = nextElement(&iter))
  {
    //wpt for stroing wpt info
    node = xmlNewChild(root_node, NULL, BAD_CAST name, NULL); // change cases for each
    //node for wpt info
    // for storing each lon/lat in the iteration
    sprintf(lat, "%f", wpt->latitude);
    sprintf(lon, "%f", wpt->longitude);
    //set lat
    xmlNewProp(node, BAD_CAST "lat", BAD_CAST lat);
    //set lon
    xmlNewProp(node, BAD_CAST "lon", BAD_CAST lon);
    //for name
    if (strlen(wpt->name) != 0)
    {
      wptnode = xmlNewChild(node, NULL, BAD_CAST "name", NULL);
      info = xmlNewText(BAD_CAST wpt->name);
      xmlAddChild(wptnode, info);
    }
    //check for other data using an iterator
    ListIterator iterwpt = createIterator(wpt->otherData);
    for (GPXData *other = nextElement(&iterwpt); other != NULL; other = nextElement(&iterwpt))
    {
      wptnode = xmlNewChild(node, NULL, BAD_CAST other->name, NULL);
      info = xmlNewText(BAD_CAST other->value);
      xmlAddChild(wptnode, info);
    }
    if (strlen(wpt->name) == 0 || getLength(wpt->otherData) == 0)
    {
      info = xmlNewText(BAD_CAST "");
      xmlAddChild(node, info);
    }
  }
}
///////////////////////////////////////////////////////////////////////////////////////////////////
void docWriter(GPXdoc *doc, xmlDocPtr *xmlDocPt)
{
  xmlDocPtr xmlDoc = NULL;
  // TODO: check for NULLS ns , list , creator , version , namespace
  xmlNodePtr root_node = NULL, node = NULL, wptnode = NULL, info = NULL; // node pointer
  xmlDoc = xmlNewDoc(BAD_CAST "1.0");                                    // check
  //printf("docWriter: %p\n", xmlDoc);
  // set the root node
  root_node = xmlNewNode(NULL, BAD_CAST "gpx");

  // set the next node
  // set the root element in the file
  char version[100];
  // set the creator
  xmlNewProp(root_node, BAD_CAST "creator", BAD_CAST doc->creator);
  // set the version
  sprintf(version, "%.1f", doc->version);
  xmlNewProp(root_node, BAD_CAST "version", BAD_CAST version);
  // set the namespace
  xmlNsPtr namespace = xmlNewNs(root_node, BAD_CAST doc->namespace, BAD_CAST NULL);
  xmlSetNs(root_node, namespace);
  xmlDocSetRootElement(xmlDoc, root_node);
  node = xmlNewChild(root_node, NULL, NULL, NULL);
  // for Waypoints
  waypointMaker(doc->waypoints, node, root_node, "wpt");
  //void xmltoWaypoint(doc,node,wptnode,info,)
  //create an iterator to get the next elemets
  ListIterator iter2 = createIterator(doc->routes);
  for (Route *rte = nextElement(&iter2); rte != NULL; rte = nextElement(&iter2))
  {
    //wpt for stroing wpt info
    node = xmlNewChild(root_node, NULL, BAD_CAST "rte", NULL); // change cases for each
    //node for wpt info
    if (rte->name != NULL)
    {
      wptnode = xmlNewChild(node, NULL, BAD_CAST "name", NULL);
      info = xmlNewText(BAD_CAST rte->name);
      xmlAddChild(wptnode, info);
    }
    //check for other data using an iterator
    ListIterator iterrte = createIterator(rte->otherData);
    for (GPXData *other = nextElement(&iterrte); other != NULL; other = nextElement(&iterrte))
    {
      wptnode = xmlNewChild(node, NULL, BAD_CAST other->name, NULL);
      info = xmlNewText(BAD_CAST other->value);
      xmlAddChild(wptnode, info);
    }
    //declare node child for route
    xmlNodePtr rtenode = NULL; //xmlNewChild(node,BAD_CAST "gpx");;
    // for waypoints in route
    waypointMaker(rte->waypoints, rtenode, node, "rtept");


  } //create an iterator to get the next elemets
  ListIterator iter3 = createIterator(doc->tracks);
  for (Track *trk = nextElement(&iter3); trk != NULL; trk = nextElement(&iter3))
  {
    //wpt for storing wpt info
    node = xmlNewChild(root_node, NULL, BAD_CAST "trk", NULL); // change cases for each
    //node for wpt info
    if (strlen(trk->name) != 0)
    {
      wptnode = xmlNewChild(node, NULL, BAD_CAST "name", NULL);
      info = xmlNewText(BAD_CAST trk->name);
      xmlAddChild(wptnode, info);
    }

    // //check for other data using an iterator
    ListIterator iterrte = createIterator(trk->otherData);
    for (GPXData *other = nextElement(&iterrte); other != NULL; other = nextElement(&iterrte))
    {
      wptnode = xmlNewChild(node, NULL, BAD_CAST other->name, NULL);
      info = xmlNewText(BAD_CAST other->value);
      xmlAddChild(wptnode, info);
    }

    // for wpt in track segments

    ListIterator iterseg = createIterator(trk->segments);
    for (TrackSegment *seg = nextElement(&iterseg); seg != NULL; seg = nextElement(&iterseg))
    {
      xmlNodePtr segnode = xmlNewChild(node, NULL, BAD_CAST "trkseg", NULL);
      // for child node in seg
      xmlNodePtr trkptnode = NULL;
      // for waypoints in track
      waypointMaker(seg->waypoints, trkptnode, segnode, "trkpt");
    }
  }
  *xmlDocPt = xmlDoc;
}
/////////////////////////////////////////////////////////////////////////////////////////////////////////
bool writeGPXdoc(GPXdoc *doc, char *fileName)
{
  if (fileName == NULL)
  {
    return false;
  }
  xmlDocPtr xmlDoc = NULL; //xml document pointer

  docWriter(doc, &xmlDoc); // initialize it

  if (xmlSaveFormatFileEnc(fileName, xmlDoc, "UTF-8", 1) != -1)
  {
    //xmlSaveFormatFileEnc(fileName,xmlDoc,"UTF-8",1);
    return true;
  }
  return false;
}
/////////////////////////////////////////////////////////////////////////////////////////////////////////
bool validateGPXDoc(GPXdoc *doc, char *gpxSchemaFile)
{
  if (doc == NULL || gpxSchemaFile == NULL)
  {
    printf("\nGOT NULL\n");
    return false;
  }
  // create the xml doc
  xmlDocPtr xmlDoc = NULL;
  docWriter(doc, &xmlDoc); // initialize it
  // check the xml doc
  int check = 0;
  int validityCheck = validater(xmlDoc, gpxSchemaFile); //validater
  if (validityCheck != 0)
  {
    return false;
  }
  if (strlen(doc->namespace) > 0 && doc->version != 0.0 && doc->creator != NULL && doc->waypoints != NULL && doc->routes != NULL && doc->tracks != NULL){
    check = 1;
  }

  if (validityCheck == 0 && check == 1)
  {
    return true;
  }

  return false;
}
////////////////////////////////////////////////Module 2/////////////////////////////////////////////////////////
float round10(float len)
{
  int num = (len + 5) / 10;
  num *= 10;
  return num;
}
//////////////////////////////////////////helper functions//////////////////////////////////////////////////////////////////////
double haversine(double firstLat, double firstLon, double secondLat, double secondLon)
{ // in meters and formula is gotten from website given in assignment
  const float latitudeOriginal  = firstLat * (M_PI / 180);
  const float latitudeOriginal2 = secondLat* (M_PI / 180);

  const float newLat = (secondLat- firstLat) * M_PI / 180;
  const float newLon = (secondLon - firstLon) * M_PI / 180;

  const float a = sin(newLat / 2) * sin(newLat / 2) + cos(latitudeOriginal) * cos(latitudeOriginal2) * sin(newLon / 2) * sin(newLon / 2);
  const float c = 2 * atan2(sqrt(a), sqrt(1 - a));
  const int R = 6371 * pow(10, 3);
  const float d = R * c;
  return d;
}

double waypointNum(List *doc)
{
  double lat1, lon1, lat2, lon2 = 0;
  double num = 0;
  ListIterator iter = createIterator(doc);
  for (Waypoint *wpt = nextElement(&iter); wpt != NULL;)
  {
    //first pair

    if (wpt != NULL)
    {
      lat1 = wpt->latitude;
      lon1 = wpt->longitude;
    }
    // onto the next pair
    wpt = nextElement(&iter);
    //second pair
    if (wpt != NULL)
    {
      lat2 = wpt->latitude;
      lon2 = wpt->longitude;
      //use the formula for distance
    }
    num += haversine(lat1, lon1, lat2, lon2);
  }
  return num;
}
///////////////////////////////////////////get the route legth/////////////////////////////////////////////////////////
float getRouteLen(const Route *rt)
{
  float ret;
  if (rt == NULL)
  {
    return 0;
  }
  ret = waypointNum(rt->waypoints);
  return ret;
}
///////////////////////////////////////////////////get the track len/////////////////////////////////////////////////////////////
double segLen(List *doc)
{
  ListIterator iter = createIterator(doc);
  float sum = 0;
  TrackSegment *current;
  TrackSegment *next = NULL;
  Waypoint *first;
  while ((current = nextElement(&iter)) != NULL)
  {
    ListIterator iter2 = iter;
    next = nextElement(&iter2);
    Waypoint *last = getFromBack(current->waypoints);
    sum += waypointNum(current->waypoints);
    //  printf("%f\n", sum);
    if (next != NULL)
    {
      first = getFromFront(next->waypoints);
      sum += waypointNum(next->waypoints);
      sum += haversine(last->latitude, last->longitude, first->latitude, first->latitude);
    }
  }
  return sum;
}
/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
float getTrackLen(const Track *tr)
{
  float ret = 0;
  if (tr == NULL)
  {
    return 0;
  }
  ret += segLen(tr->segments);
  return ret;
}
/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
int numRoutesWithLength(const GPXdoc *doc, float len, float delta)
{
  double numroutelen;
  int counter = 0;
  if (doc == NULL || len < 0 || delta < 0)
  {
    return 0;
  }
  ListIterator iter = createIterator(doc->routes);
  for (Route *rte = nextElement(&iter); rte != NULL; rte = nextElement(&iter))
  {
    numroutelen = getRouteLen(rte);
    if (abs(len - numroutelen) <= delta)
    {
      counter++;
    }
  }
  return counter;
}
/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
int numTracksWithLength(const GPXdoc *doc, float len, float delta)
{
  double numtracklen;
  int counter = 0;
  if (doc == NULL || len < 0 || delta < 0)
  {
    return 0;
  }
  ListIterator iter = createIterator(doc->tracks);
  for (Track *trk = nextElement(&iter); trk != NULL; trk = nextElement(&iter))
  {
    numtracklen = getTrackLen(trk);
    if (abs(len - numtracklen) <= delta)
    {
      counter++;
    }
  }
  return counter;
}
/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
double calc_dist(List *waypt)
{
  double sum = 0;
  Waypoint *first;
  Waypoint *last = getFromBack(waypt);
  first = getFromFront(waypt);
  sum += haversine(last->latitude, last->longitude, first->latitude, first->latitude);
  return sum;
}
bool isLoopRoute(const Route *route, float delta)
{
  if (route == NULL || delta < 0)
  {
    return 0;
  }
  int waypointLen = getLength(route->waypoints);
  double DistCheck = calc_dist(route->waypoints);
  if (waypointLen > 3 && DistCheck <= delta)
  {
    return true;
  }
  return 0;
}
/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
bool isLoopTrack(const Track *tr, float delta)
{
  if (tr == NULL || delta < 0)
  {
    return false;
  }
  Waypoint *first, *last;
  int waypointLen = 0;
  ListIterator iter = createIterator(tr->segments);
  for (TrackSegment *trkseg = nextElement(&iter); trkseg != NULL; trkseg = nextElement(&iter)){
    waypointLen += getLength(trkseg->waypoints);// getLength()
  }
  first = getFromFront(((TrackSegment *)getFromFront(tr->segments))->waypoints);
  last = getFromBack(((TrackSegment *)getFromBack(tr->segments))->waypoints);
  if (first == NULL || last == NULL)
  {
    return false;
  }
  double DistCheck = haversine(first->latitude, first->longitude, last->latitude, last->longitude); //calc_dist(tr->segments); //ask
  if (waypointLen > 3 && DistCheck <= delta)
  {
    return true;
  }
  return false;
}
void dumDelete(void *data)
{
}
/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
List *getRoutesBetween(const GPXdoc *doc, float sourceLat, float sourceLong, float destLat, float destLong, float delta)
{
  if ( doc == NULL || delta == -1)
  {
    return NULL;
  }
  List *rt = initializeList(&routeToString, &dumDelete, &compareRoutes);
  ListIterator iter = createIterator(doc->routes);
  Waypoint *first, *last;
  for (Route *rte = nextElement(&iter); rte != NULL; rte = nextElement(&iter))
  {
    first = getFromFront(rte->waypoints);
    last = getFromBack(rte->waypoints);
    double d1 = haversine(first->latitude, first->longitude, sourceLat, sourceLong);
    double d2 = haversine(last->latitude, last->longitude, destLat, destLong);
    if (d1 <= delta && d2 <= delta)
    {
      insertBack(rt, rte);
    }
  }
  if (getLength(rt) == 0)
  {
    return NULL;
  }

  return rt;
}
/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
List *getTracksBetween(const GPXdoc *doc, float sourceLat, float sourceLong, float destLat, float destLong, float delta)
{
  if ( doc == NULL || delta == -1)
  {
    return NULL;
  }
  List *tr = initializeList(&trackToString, &dumDelete, &compareTracks);
  ListIterator iter = createIterator(doc->tracks);
  Waypoint *first, *last;
  for (Track *trk = nextElement(&iter); trk != NULL; trk = nextElement(&iter))
  {

    first = (Waypoint *)getFromFront(((TrackSegment *)getFromFront(trk->segments))->waypoints);
    last = (Waypoint *)getFromBack(((TrackSegment *)getFromBack(trk->segments))->waypoints);

    double d1 = haversine(first->latitude, first->longitude, sourceLat, sourceLong);
    double d2 = haversine(last->latitude, last->longitude, destLat, destLong);
    if (d1 <= delta && d2 <= delta)
    {
      insertBack(tr, trk);
    }
  }
  // if the len is 0 , return NULL
  if (getLength(tr) == 0)
  {
    return NULL;
  }

  return tr;
} // done validation
///////////////////////////////////////////////////////////Module 3//////////////////////////////////////////////////////////////////////
char *routeToJSON(const Route *rt)
{
  char *str = malloc(sizeof(char) * 10000);
  if (rt == NULL)
  {
    strcpy(str, "{}");
    return str;
  }
  char name[50], loopStat[50];
  if (rt->name == NULL || strlen(rt->name) == 0)
  {
    strcpy(name, "none");
  }
  else
  {
    strcpy(name, rt->name);
  }
  if (isLoopRoute(rt, 10) == true)
  {
    strcpy(loopStat, "true");
  }
  else
  {
    strcpy(loopStat, "false");
  }
  float len = getRouteLen(rt);
  len = round10(len);
  //numwaypoints
  int numwaypt = getLength(rt->waypoints);
  sprintf(str, "{\"name\":\"%s\",\"numPoints\":%d,\"len\":%.1f,\"loop\":%s}", name, numwaypt, len, loopStat);
  return str;
}
/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
char *trackToJSON(const Track *tr)
{
  char *str = malloc(sizeof(char) * 256);
  if (tr == NULL)
  {
    strcpy(str, "{}");
    return str;
  }
  char name[50], loopStat[50];
  if (tr->name == NULL || strlen(tr->name) == 0)
  {
    strcpy(name, "none");
  }
  else
  {
    strcpy(name, tr->name);
  }
  if (isLoopTrack(tr, 10) == true)
  {
    strcpy(loopStat, "true");
  }
  else
  {
    strcpy(loopStat, "false");
  }
  float len = getTrackLen(tr);
  len = round10(len);
  //numwaypoints
  int numwaypt = 0;
  ListIterator iter = createIterator((List *)tr->segments);
  for ( TrackSegment * trkseg = nextElement(&iter); trkseg != NULL; trkseg = nextElement(&iter))
  {
    numwaypt += getLength(trkseg->waypoints);
  }
  //str = realloc(str,strlen(str)+strlen("{\"name\":\"")+1);
  strcpy(str,"{\"name\":\"");
  str = realloc(str,strlen(str)+strlen(name)+1);
  strcat(str,name);
  str = realloc(str,strlen(str)+strlen(",\"numPoints\":")+1);
  strcat(str,"\",\"numPoints\":");
  // str = realloc(str,strlen(str)+strlen((char*)numwaypt)+1);
  // strcat(str,(char*)numwaypt);
  char *wpt = malloc(sizeof(char) * 2000);
  sprintf(wpt,"%d",numwaypt);
  str = realloc(str,strlen(str)+strlen(wpt)+1);
  strcat(str,wpt);
  str = realloc(str,strlen(str)+strlen(",\"len\":")+1);
  strcat(str,",\"len\":");
  // str = realloc(str,strlen(str)+strlen((char*)len)+1);
  // strcat(str,(char*)len);
  char *le = malloc(sizeof(char) * 256);
  sprintf(le,"%.1f",len);
  str = realloc(str,strlen(str)+strlen(le)+1);
  strcat(str,le);
  str = realloc(str,strlen(str)+strlen(",\"loop\":")+1);
  strcat(str,",\"loop\":");
  str = realloc(str,strlen(str)+strlen(loopStat)+1);
  strcat(str,loopStat);
  str = realloc(str,strlen(str)+strlen("}")+1);
  strcat(str,"}");
  //sprintf(str, "{\"name\":\"%s\",\"numPoints\":%d,\"len\":%.1f,\"loop\":%s}", name, numwaypt, len, loopStat);
  return str;
}
/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
char *jsonHelper(const List *list, int mode)
{
  int i = 0;
  ListIterator iter = createIterator((List *)list);
  char *oldstr = malloc(sizeof(char) * 10);
  strcpy(oldstr, "[");
  if (mode == 1)
  {
    for (Route *rte = nextElement(&iter); rte != NULL; rte = nextElement(&iter))
    {
      if (i!=0) {
        oldstr[strlen(oldstr)]=',';
        oldstr[strlen(oldstr)+1]='\0';
      }
      char *tmp = routeToJSON(rte);
      int length = strlen(tmp);
      oldstr = realloc(oldstr, sizeof(char) * length + 10 + (2*i) + strlen(oldstr)); //+10 already in old
      strcat(oldstr,tmp);
      free(tmp);
      i++;
    }
    strcat(oldstr, "]");
    return oldstr;
  }
  else if (mode == 2)
  {
    for (Track *rte = nextElement(&iter); rte != NULL; rte = nextElement(&iter))
    {
      if (i!=0) {
        oldstr[strlen(oldstr)]=',';
        oldstr[strlen(oldstr)+1]='\0';
      }
      char *tmp = trackToJSON(rte);
      int length = strlen(tmp);
      oldstr = realloc(oldstr, sizeof(char) * length + 10 + (2*i) + strlen(oldstr)); //+10 already in old
      strcat(oldstr,tmp);
      free(tmp);
      i++;
    }
    strcat(oldstr, "]");
    printf("%s",oldstr);
    return oldstr;
  }
  strcpy(oldstr, "[]");
  return oldstr;
}
/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
char *routeListToJSON(const List *list)
{
  int mode = 1;
  if (list == NULL)
  {
    return "[]";
  }

  return jsonHelper(list, mode);
}
/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
char *trackListToJSON(const List *list)
{
  int mode = 2;
  if (list == NULL)
  {
    return "[]";
  }
  return jsonHelper(list, mode);
}
/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
char *GPXtoJSON(const GPXdoc *gpx)
{
  if (gpx == NULL)
  return "{}";
  char *str = malloc(sizeof(char) * 255);
  sprintf(str, "{\"version\":%.2f,\"creator\":\"%s\",\"numWaypoints\":%d,\"numRoutes\":%d,\"numTracks\":%d}", gpx->version, gpx->creator,
  getLength(gpx->waypoints), getLength(gpx->routes), getLength(gpx->tracks));
  return str;
}

////////////////////////////////////////////////////////Bonus functions are not implemented and are just declared/////////////////////////////////////////////////////////////////////////
void addWaypoint(Route *rt, Waypoint *pt)
{
  if (pt == NULL || rt == NULL) {
         return;
     }
     insertBack(rt->waypoints, pt);
}
void addRoute(GPXdoc *doc, Route *rt)
{
  if (doc == NULL || rt == NULL) {
         return;
     }
     insertBack(doc->routes, rt);
}
/////////////////////////////////////////////////////////////
int addingpoint(char *filename,char *name, double lat, double lon)
{
  GPXdoc *doc = createValidGPXdoc(filename, "gpx.xsd");
  if (doc == NULL)
  {
    //printf("here 697 \n");
    return 0;
  }
  Route *rte = calloc(1,sizeof(Route));
  rte->name = calloc(500,sizeof(char));
  strcpy(rte->name,name);
  rte->waypoints = initializeList(&waypointToString, &deleteWaypoint, &compareWaypoints);
  rte->otherData = initializeList(&gpxDataToString, &deleteGpxData, &compareGpxData);

  Waypoint *wpt =  calloc(1, sizeof(Waypoint));
  wpt->latitude = lat;
  wpt->longitude = lon;
  wpt->name = calloc(500, sizeof(char));
  wpt->otherData = initializeList(&gpxDataToString, &deleteGpxData, &compareGpxData);

  insertBack(rte->waypoints,wpt);
  insertBack(doc->routes, rte);
  if (writeGPXdoc(doc, filename) == false)
  {
    deleteGPXdoc(doc);
    return 0;
  }
   deleteGPXdoc(doc);
  printf("here 721 \n");
  return 1;
}
GPXdoc *JSONtoGPX(const char *gpxString)
{
  GPXdoc *doc = NULL;
  return doc;
}

Waypoint *JSONtoWaypoint(const char *gpxString)
{
  return NULL;
}

Route *JSONtoRoute(const char *gpxString)
{
  return NULL;
}
//////////////////////////////////////////////////////////
char * retSTR(char * filename){
  GPXdoc * doc = createValidGPXdoc(filename,"gpx.xsd");
  char * str = GPXtoJSON(doc);
  deleteGPXdoc(doc);
  return str;
}
///////////////////////////////////////////////////////////
char * retRoute(char * filename){
  GPXdoc * doc = createValidGPXdoc(filename,"gpx.xsd");
  char * str = routeListToJSON(doc->routes);
  deleteGPXdoc(doc);
  return str;
}
////////////////////////////////////////////////////////////
char * retTrack(char * filename){
  GPXdoc * doc = createValidGPXdoc(filename,"gpx.xsd");
  char * str = trackListToJSON(doc->tracks);
  deleteGPXdoc(doc);
  return str;
}
int retFile(char *fileName, double  version, char* creator){
  GPXdoc *Doc = malloc(sizeof(GPXdoc));
  if (Doc == NULL) {
    return 0;
  }
  //int size = strlen(creator);
  strcpy(Doc->namespace, "http://www.topografix.com/GPX/1/1");
  Doc->version = version;
  Doc->creator = malloc(strlen(creator)+1);
  strcpy(Doc->creator, creator);
  Doc->waypoints = initializeList(&waypointToString, &deleteWaypoint, &compareWaypoints);
  Doc->routes = initializeList(&routeToString, &deleteRoute, &compareRoutes);
  Doc->tracks = initializeList(&trackToString, &deleteTrack, &compareTracks);
  int value  = validateGPXDoc(Doc,"gpx.xsd");
  if ( writeGPXdoc(Doc,fileName) == true && value == true) {
    return 1;
  }
  //printf("it dont work \n");
  return 0;
}
//getRoutesBetween(const GPXdoc *doc, float sourceLat, float sourceLong, float destLat, float destLong, float delta)
// char * arraytoret(char * fileName,float sourceLat, float sourceLong, float destLat, float destLong, float delta){
//   GPXdoc *doc = createValidGPXdoc(fileName, "gpx.xsd");
//   if (doc == NULL)
//   {
//     return 0;
//   }
//   //initialize route and tracks
//   Route *rte = calloc(1,sizeof(Route));
//   rte->waypoints = initializeList(&waypointToString, &deleteWaypoint, &compareWaypoints);
//   rte->otherData = initializeList(&gpxDataToString, &deleteGpxData, &compareGpxData);
//   Track * track = malloc(sizeof(Track));
//   track->segments = initializeList(&trackSegmentToString, &deleteTrackSegment, &compareTrackSegments);
//   track->otherData = initializeList(&gpxDataToString, &deleteGpxData, &compareGpxData);
//   // us the between functions
//   List * rt = getRoutesBetween(doc, sourceLat, sourceLong, destLat, destLong, delta);
//   List * trk = getTracksBetween(doc,sourceLat,sourceLong, destLat,destLong, delta);
//   // covert them into JSONs
//   char * rtPath = routeListToJSON(rt);
//   char * trkPath = trackListToJSON(trk);
//   // make sure it prints rihgts
//   char * string = malloc(sizeof(char)* 256);
//   strcpy(string,"[");
//   string = realloc(string,strlen(string)+strlen(rtPath)+1);
//   strcat(string,rtPath);
//   strcat(string,",");
//   string = realloc(string,strlen(string)+strlen(trkPath)+1);
//   strcat(string,trkPath);
//   strcat(string,"]");
//   printf("%s \n",string);
//  return string;
// }

char *routeFindPath(char *filename, float startLat, float startLon, float endLat, float endLon, float accuracy) {
  GPXdoc *doc = createValidGPXdoc(filename, "gpx.xsd");
  List *routeList = getRoutesBetween(doc, startLat, startLon, endLat, endLon, accuracy);
  char *str = routeListToJSON(routeList);
  printf("%s\n", str);
  deleteGPXdoc(doc);
  return str;
}

char *trackFindPath(char *filename, float startLat, float startLon, float endLat, float endLon, float accuracy) {
  GPXdoc *doc = createValidGPXdoc(filename, "gpx.xsd");
  List *trackList = getTracksBetween(doc, startLat, startLon, endLat, endLon, accuracy);
  char *str = routeListToJSON(trackList);
  printf("%s\n", str);
  deleteGPXdoc(doc);
  return str;
}

int changeName(char* filename, char* oldName, char* newName) {
    GPXdoc *doc = createValidGPXdoc(filename, "gpx.xsd");
    Route *rt = getRoute(doc, oldName);
    Track *tr = getTrack(doc, oldName);
    int retval = 0;
    if (rt != NULL) {
        strcpy(rt->name, newName);
        writeGPXdoc(doc, filename);
        retval = 1;
    } else if (tr != NULL) {
        strcpy(tr->name, newName);
        writeGPXdoc(doc, filename);
        retval = 1;
    }

    deleteGPXdoc(doc);
    return retval;
}

char *routeOther(GPXdoc *doc, char *name) {
    Route *route = getRoute(doc, name);
    int i = 0;
    char *oldstring = malloc(300*route->otherData->length);
    ListIterator iter = createIterator(route->otherData);
    strcpy(oldstring, "[");

    for (GPXData* node = (GPXData*)nextElement(&iter); node!=NULL; node = (GPXData*)nextElement(&iter)){
        char *string = malloc(sizeof(char)*300);
        sprintf(string, "{\"Name\":\"%s\", \"Value\":\"%s\"}", node->name, node->value);

        if (i != 0) {
            strcat(oldstring, ",");
        }

        strcat(oldstring, string);
        free(string);

        i++;
	  }
    strcat(oldstring, "]");
    return oldstring;
}


char *trackOther(GPXdoc *doc, char *name) {
    Track *track = getTrack(doc, name);
    int i = 0;
    char *oldstring = malloc(300*track->otherData->length);
    ListIterator iter = createIterator(track->otherData);
    strcpy(oldstring, "[");

    for (GPXData* node = (GPXData*)nextElement(&iter); node!=NULL; node = (GPXData*)nextElement(&iter)){
        char *string = malloc(sizeof(char)*300);
        sprintf(string, "{\"Name\":\"%s\", \"Value\":\"%s\"}", node->name, node->value);

        if (i != 0) {
            strcat(oldstring, ",");
        }

        strcat(oldstring, string);
        free(string);

        i++;
	  }
    strcat(oldstring, "]");
    return oldstring;
}


char *showOther(char *filename, char *name) {
  GPXdoc *doc = createValidGPXdoc(filename, "gpx.xsd");
    Route *rt = getRoute(doc, name);
    Track *tr = getTrack(doc, name);
    char *retval;
    if (rt != NULL) {
      retval = routeOther(doc, name);
    } else if (tr != NULL) {
      retval = trackOther(doc, name);
    }

    deleteGPXdoc(doc);
    return retval;
}

#include <stdio.h>     
#include <stdlib.h>  
#include <libxml/parser.h>
#include <libxml/tree.h>
#include "GPXParser.h"
#include <stdbool.h> 
/**
 * Add additional information to GPXData
 * 
 * TODO: potential cause for strange outputs and seg fault 
 */ 
GPXData * dataConstructor(xmlNode *node) {
    GPXData *data = malloc(sizeof(GPXData) + sizeof(char) * 256);
    if (data->name != NULL || strcpy((char *)data->name,"")!= 0)
    {
     strcpy(data->name, (char *)node->name);
    }
    xmlChar * value = xmlNodeGetContent(node);
    if (value != NULL || strcpy((char *)value,"")!= 0)
    {
     strcpy(data->value, (char *)value);
    }
    xmlFree(value);
    return data;
}
/////////////////////////////////////////////////////////////////////////////////////////////////////////
/**
 * Create a wapoint.
 * 
 */
Waypoint * waypointConstructor(xmlNode *node) {
    Waypoint * wpt = malloc(sizeof(Waypoint));
    wpt->name = calloc(256, sizeof(char));
    strcpy((char *)wpt->name,"");
    wpt->otherData = initializeList(&gpxDataToString, &deleteGpxData, &compareGpxData);
    wpt->latitude = 0;
    wpt->longitude = 0;
    // initialized
    xmlAttr *attr;

    for (attr = node->properties; attr != NULL; attr = attr->next)
    {
        xmlNode *value = attr->children;
        
        char *attrName = (char *)attr->name;
        char *cont = (char *)(value->content);
        if (strcmp(attrName,"lat")== 0){ //checking the name
            wpt->latitude = atof(cont);
        } else if (strcmp(attrName,"lon")== 0){ //checking the name
            wpt->longitude = atof(cont);
        }
    }
    
    for (xmlNode *child = node->children; child; child = child->next) {
        if (strcmp((char*)child->name,"name")==0)
        {
            xmlChar *content = xmlNodeGetContent(child); // n
            //printf("Waypoint name<%s>: %s \n", child->name, content);
            if (content != NULL) {
                strcpy(wpt->name, (char *)content);
            }
            xmlFree(content);
        }else if (strcmp((char*)child->name,"text")!=0) {
            //printf("Other <%s>: %s \n", child->name, xmlNodeGetContent(child));
            GPXData *data = dataConstructor(child);
            insertBack(wpt->otherData, data);
        }
        
    }
    // do it inside the if strcmp statement
    //insertBack(wpt->otherData, data);
    return wpt;
}
////////////////////////////////////////////////////////////////////////////////////////////////////////
TrackSegment * trksegConstructor(xmlNode *node) {
    TrackSegment * trackseg = malloc(sizeof(TrackSegment));
    trackseg->waypoints = initializeList(&waypointToString,&deleteWaypoint,&compareWaypoints);
    xmlNode *child;
    for (child = node->children; child; child = child->next) {
        if (strcmp((char*)child->name,"trkpt")==0)
        {
            Waypoint *wpt = waypointConstructor(child);
            insertBack(trackseg->waypoints, wpt);
            
        }
        
    }
    return trackseg;
   }
////////////////////////////////////////////////////////////////////////////////////////////////////////
/**
 * Create a Route representation for rte node.
 * 
 * 
 */ 
Route * rteConstructor(xmlNode *node) {
    Route * route = malloc(sizeof(Route));
    route->name = malloc(255);
    strcpy((char *)route->name,"");
    route->waypoints = initializeList(&waypointToString,&deleteWaypoint,&compareWaypoints);
    route->otherData = initializeList(&gpxDataToString, &deleteGpxData, &compareGpxData);
    xmlNode *child;
    for (child = node->children; child; child = child->next) {
        if (strcmp((char*)child->name,"name")==0)
        {
            xmlChar *content = xmlNodeGetContent(child);
            //printf("<%s>: %s \n", child->name, content);
            if (content != NULL) {
                strcpy(route->name, (char *)content);
            }
            xmlFree(content);
            
        }
        else if (strcmp((char*)child->name,"rtept")==0) {
            Waypoint *wpt = waypointConstructor(child);
            insertBack(route->waypoints, wpt);
            
        }
        else if(strcmp((char*)child->name,"text")!=0) {
            //printf("Other <%s>: %s \n", child->name, xmlNodeGetContent(child));
            GPXData *data = dataConstructor(child);
            insertBack(route->otherData, data);
        }
        
    }
    return route;
}
////////////////////////////////////////////////////////////////////////////////////////////////////////
Track * trkConstructor(xmlNode *node) {
    Track * track = malloc(sizeof(Track));
    track->name = malloc(255);
    strcpy((char *)track->name,"");
    track->segments = initializeList(&trackSegmentToString, &deleteTrackSegment, &compareTrackSegments);
    track->otherData = initializeList(&gpxDataToString, &deleteGpxData, &compareGpxData);
    xmlNode *child;
    for (child = node->children; child; child = child->next) {
        if (strcmp((char*)child->name,"name")==0)
        {
            xmlChar *content = xmlNodeGetContent(child);
            //printf("<%s>: %s \n", child->name, content);
            if (content != NULL) {
                strcpy(track->name, (char *)content);
            }
            xmlFree(content);
            
        }
        else if (strcmp((char*)child->name,"trkseg")==0) {
            TrackSegment * trkSeg = trksegConstructor(child);
            insertBack(track->segments,trkSeg);
            
        }
        else if(strcmp((char*)child->name,"text")!=0) {
            //printf("Other <%s>: %s \n", child->name, xmlNodeGetContent(child));
            GPXData *data = dataConstructor(child);
            insertBack(track->otherData, data);
        }
        
    }
    return track;
}
//////////////////////////////////waypoint functions//////////////////////////////////////////////////////////////////////
char * waypointToString(void* data) {
    if (data == NULL){
		return NULL;
	}
    Waypoint* wpt = (Waypoint *) data;
    char * str = malloc(sizeof(char)* 1000);
    char * otherData = toString(wpt->otherData);
    sprintf(str, "Name: %s, lat: %f lon: %f, Other: %s", wpt->name, wpt->latitude, wpt->longitude, otherData);
    free(otherData);
    return str;
}
void deleteWaypoint(void* data){
    Waypoint* wpt = (Waypoint *) data;
    free(wpt->name);
    freeList(wpt->otherData);
    free(wpt);
}
// 0: parameters are equal
// > 0 : first > second
// < 0: first < second
// Compares name, latitude nad longitude respectively
int compareWaypoints(const void *first, const void *second) {
    Waypoint* wpt_1 = (Waypoint *) first;
    Waypoint* wpt_2 = (Waypoint *) second;

    char * one = waypointToString(wpt_1);
    char * two = waypointToString(wpt_2);
    
    int ret  = strcmp(one,two);
    free(one);
    free(two);
    if (ret == 0)
    {
        return 0;
    }

    return 1;
    
}
/////////////////////////////////////////////route functions///////////////////////////////////////////////////////////
void deleteRoute(void* data){
Route* rt = (Route *) data;
    free(rt->name);
    freeList(rt->waypoints);
    freeList(rt->otherData);
    free(rt);
}
char* routeToString(void* data){
      if (data == NULL){
		return NULL;
	}
Route * rt = (Route *) data;
    char * str = malloc(sizeof(char)* 1000);
    // what to do for waypoints
    char * wpt = toString(rt->waypoints);
    char * otherData = toString(rt->otherData);
    sprintf(str, "Name: %s\n Waypoint: %s \n Other Data: %s", rt->name, wpt, otherData);
    free(wpt);
    free(otherData);
    return str;

}
int compareRoutes(const void *first, const void *second){
    if (first == NULL || second == NULL) 
    return 0;
    Route* rt_1 = (Route *) first;
    Route* rt_2 = (Route *) second;
    char * one = routeToString(rt_1);
    char * two = routeToString(rt_2);
    int flag =strcmp(one,two);
    free(one);
    free(two);
return flag;
}
//////////////////////////////////////////////track functions//////////////////////////////////////////////////////////
void deleteTrack(void* data){
Track * trk = (Track *) data;
    free(trk->name);
    freeList(trk->segments);
    freeList(trk->otherData);
    free(trk);
}
char* trackToString(void* data){
      if (data == NULL){
		return NULL;
	}
Track * trk = (Track *) data;
    char * str = malloc(sizeof(char)* 1000);
    char * segments = toString(trk->segments);
    char * otherData = toString(trk->otherData);
    sprintf(str, "Name: %s\n Segments: %s \n Other Data: %s", trk->name, segments, otherData);
    free(segments);
    free(otherData);
    return str;
}
int compareTracks(const void *first, const void *second){
Track * trk_1 = (Track *) first;
Track * trk_2 = (Track *) second;
char * one = trackToString(trk_1);
char * two = trackToString(trk_2);
int flag =strcmp(one,two);
free(one);
free(two);
return flag;
}

///////////////////////////////////////////Track Segment functions/////////////////////////////////////////////////////////////
void deleteTrackSegment(void* data){
TrackSegment * trkseg = (TrackSegment *) data;
    freeList(trkseg->waypoints);
    free(trkseg);

}
char* trackSegmentToString(void* data){
      if (data == NULL){
		return NULL;
	}
TrackSegment * trk = (TrackSegment *) data;
    char * str = malloc(sizeof(char)* 1000);
    // what to do for tracksegments
    char * wpt = toString(trk->waypoints);
    sprintf(str, "Waypoints: %s", wpt);
    free(wpt);
    return str;
}
int compareTrackSegments(const void *first, const void *second){
    TrackSegment * trk_1 = (TrackSegment *) first;
    TrackSegment * trk_2 = (TrackSegment *) second;
    char * one = trackSegmentToString(trk_1);
    char * two = trackSegmentToString(trk_2);
    int ret  = strcmp(one,two);
    free(one);
    free(two);
    return ret;

}
//////////////////////////////////////////////////Gpx data function//////////////////////////////////////////////////////
void deleteGpxData( void* data){
    GPXData * gpxData = (GPXData  *) data;
    free(gpxData);

}
char* gpxDataToString( void* data){
      if (data == NULL){
		return NULL;
	}
    GPXData * gpxData = (GPXData  *) data;
    char * str = malloc(sizeof(char)* 1000);
    // what to do for tracksegments
   
    sprintf(str, "Name: %s, Value: %s", gpxData->name, gpxData->value);
    return str;
}
int compareGpxData(const void *first, const void *second){
GPXData * trk_1 = (GPXData *) first;
GPXData * trk_2 = (GPXData *) second;
char * one = gpxDataToString(trk_1);
char * two = gpxDataToString(trk_2);
int ret  = strcmp(one,two);
free(one);
free(two);
return ret;

}
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
GPXdoc* createGPXdoc(char* fileName){
// takes in the name of the file

    xmlDoc *doc;
    xmlNode *root_element;

    doc = xmlReadFile(fileName, NULL, 0);

    if (doc == NULL) {
        //printf("error: could not parse file %s\n", fileName);
        return NULL;
    }

    /*Get the root element node */
    root_element = xmlDocGetRootElement(doc);
    if (root_element == NULL) {
       // printf("error: root element couldn't be found \n");
        return NULL;
    }
    GPXdoc *gpxDoc = (GPXdoc*) malloc(sizeof(GPXdoc));
    xmlNode *node;
    gpxDoc->creator = (char*) malloc(sizeof(char) * 256);
    gpxDoc->creator[0] = '\0';
    gpxDoc->version = 0; // initialize to 0 for now
    // passing a reference to these functions and not calling to them 
    // initializing everything with the function that are implemented
    gpxDoc->waypoints = initializeList(&waypointToString,&deleteWaypoint,&compareWaypoints);
    gpxDoc->routes = initializeList(&routeToString,&deleteRoute,&compareRoutes);
    gpxDoc->tracks = initializeList(&trackToString,&deleteTrack,&compareTracks);
    

    // we extract all the relevant attributes from GPX root node
    xmlAttr *attr;
    const xmlChar *ns = root_element->ns->href;
    strcpy(gpxDoc->namespace, (char *)ns);
    //printf("Namespace: %s\n", ns);
    
    for (attr = root_element->properties; attr != NULL; attr = attr->next)
    {
        xmlNode *value = attr->children;
        // TODO: use strcpy() everywhere
        char *attrName = (char *)attr->name;
        char *cont = (char *)(value->content);
        
        if (strcmp(attrName,"version")==0) {
            gpxDoc->version = atof(cont);

        }
        if (strcmp(attrName,"creator")==0) {
            strcpy(gpxDoc->creator, cont);
        }
        
        //printf("\tattribute name: %s, attribute value = %s\n", attrName, cont);
    }
    // Go through the children of the root GPX
    for (node = root_element->children; node; node = node->next) {
        if (strcmp((char *) node->name, "rte") == 0) {
            //printf("Name <%s>\n", node->name);
            
            Route * route = rteConstructor(node);
            
             insertBack(gpxDoc->routes, route);
            
            // append route to the list gpxDoc->route
        }else if (strcmp((char *) node->name, "wpt") == 0)
        {
            Waypoint * wpt = waypointConstructor(node);
            insertBack(gpxDoc->waypoints, wpt);

        }else if (strcmp((char *) node->name, "trk") == 0)
        {
            Track * trk = trkConstructor(node);
             insertBack(gpxDoc->tracks, trk);
        }
        
        
    }

    /*free the document */
    xmlFreeDoc(doc);
    xmlCleanupParser();

    return gpxDoc;
}
////////////////////////////////////////////////////////////////////////////////////////////////////////
char* GPXdocToString(GPXdoc* doc){
      if (doc == NULL){
		return NULL;
	}
    char * str = malloc(sizeof(char)* 10000);
    char * wpt = toString(doc->waypoints);
    char * trk = toString(doc->tracks);
    char * rt = toString(doc->routes);
    sprintf(str, "Namespace: %s, Creator: %s, Version: %f, Waypoint: %s\nTrack: %s\nRoute: %s \n",doc->namespace, doc->creator, doc->version,wpt,trk,rt);
    free(wpt);
    free(trk);
    free(rt);
    return str;
}
void deleteGPXdoc(GPXdoc* doc){
    if (doc != NULL){
        free(doc->creator);
        freeList(doc->waypoints);
        freeList(doc->tracks);
        freeList(doc->routes);
        free(doc);
    }
}
////////////////////////////////////////////////////////////////////////////////////////////////////////
int getNumWaypoints(const GPXdoc* doc) {
    if (doc == NULL){
        return 0;
    } 
    int x =  getLength(doc->waypoints);
    return x;
}
//Total number of routes in the GPX file
int getNumRoutes(const GPXdoc* doc) {
    if (doc == NULL){
        return 0;
    }
    return getLength(doc->routes);
}

//Total number of tracks in the GPX file
int getNumTracks(const GPXdoc* doc) {
    if (doc == NULL){
        return 0;
    }
    return getLength(doc->tracks);
}

//Total number of segments in all tracks in the document
int getNumSegments(const GPXdoc* doc) {
    if (doc == NULL){
        return 0;
    }
    int totalSgmnts = 0;
    void* elem;
    ListIterator iter = createIterator(doc->tracks);
    while ((elem = nextElement(&iter)) != NULL){
		Track* track = (Track*)elem;
        int length = getLength(track->segments);
		totalSgmnts += length >= 0 ? length : 0;
	}
    return totalSgmnts;
}

//Total number of GPXData elements in the document
///////////////////////////helper functions///////////////////////////////
int iterWay (List * list){
int totalData = 0;
    void* elem;
    ListIterator iter = createIterator(list);
    while ((elem = nextElement(&iter)) != NULL){
		Waypoint* wpt = (Waypoint*)elem;
        if(strcmp(wpt->name,"")!=0){
            totalData+= 1;
        }
        totalData += getLength(wpt->otherData);
        //printf(" this is wpt->otherdata  %s",toString(wpt->otherData));
	}
    return totalData;
}
int iterSeg(List * list){
int totalData = 0;
    void* elem;
    ListIterator iter = createIterator(list);
    while ((elem = nextElement(&iter)) != NULL){
		TrackSegment* seg = (TrackSegment*)elem;
        totalData += iterWay(seg->waypoints);
	}
    return totalData;

}
////////////////////////////////////////////////////////
int getNumGPXData(const GPXdoc* doc) {
    if (doc == NULL){
        return 0;
    }
    int totalData = 0;
    totalData += iterWay(doc->waypoints);
    //printf(" this is the total data %d  at 1 \n \n",totalData);

    void* elem2;
    ListIterator iter2 = createIterator(doc->routes);
    while ((elem2 = nextElement(&iter2)) != NULL){
		Route* rte = (Route*)elem2;
        if(strcmp(rte->name,"")!=0 ){
            totalData+= 1;
        }
        totalData += iterWay(rte->waypoints);
        //printf(" this is rte->otherdata  %s",toString(rte->otherData));
        totalData += getLength(rte->otherData);
	}
    //printf(" this is the total data %d  at 2 \n \n",totalData);

    void* elem3;
    ListIterator iter3 = createIterator(doc->tracks);
    while ((elem3 = nextElement(&iter3)) != NULL){
		Track* trk = (Track*)elem3;
       if(strcmp(trk->name,"")!=0 ){
            totalData+= 1;
        }
        totalData += iterSeg(trk->segments);
        totalData += getLength(trk->otherData);
        //printf(" this is trk->otherdata  %s",toString(trk->otherData));
	}
    //printf(" this is the total data %d  at 3 \n \n",totalData);
    return totalData;
}

///////////////////////////////////////custom compare///////////////////////////////////////////////////////////////// 
bool compWay (const void* first,const void* second){
    Waypoint * data = (Waypoint *) first;
    char * searchRecord = (char *) second;
    // get the waypoint name and compare it with search record
    if(strcmp(data->name,searchRecord)==0){
        return true;
    }
    return false;
}
bool compTrk(const void* first,const void* second){
    Track * data = (Track *) first;
    char * searchRecord = (char *) second;
    if(strcmp(data->name,searchRecord)==0){
        return true;
    }
    return false;
}
bool compRt(const void* first,const void* second){
    Route * data = (Route *) first;
    char * searchRecord = (char *) second;
    if(strcmp(data->name,searchRecord)==0){
        return true;
    }
    return false;
}
////////////////////////////////////////////////////////////////////////////////////////////////////////
Waypoint* getWaypoint(const GPXdoc* doc, char* name){
     if (doc == NULL)
    {
        return NULL;
    }
    return findElement(doc->waypoints, compWay, name);
}
Track* getTrack(const GPXdoc* doc, char* name){
     if (doc == NULL)
    {
        return NULL;
    }
    return findElement(doc->tracks, compTrk, name);
}
Route* getRoute(const GPXdoc* doc, char* name){
     if (doc == NULL)
    {
        return NULL;
    }
    return findElement(doc->routes,compRt, name);
}

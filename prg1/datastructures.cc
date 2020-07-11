// Datastructures.cc

#include "datastructures.hh"

#include <random>
#include <cmath>
#include <stdexcept>
#include <iostream>
#include <algorithm>

std::minstd_rand rand_engine; // Reasonably quick pseudo-random generator

template <typename Type>
Type random_in_range(Type start, Type end)
{
    auto range = end-start;
    ++range;

    auto num = std::uniform_int_distribution<unsigned long int>(0, range-1)(rand_engine);

    return static_cast<Type>(start+num);
}

// Modify the code below to implement the functionality of the class.
// Also remove comments from the parameter names when you implement
// an operation (Commenting out parameter name prevents compiler from
// warning about unused parameters on operations you haven't yet implemented.)

Datastructures::Datastructures()
{
    // Replace this comment with your implementation
}

Datastructures::~Datastructures()
{
    // Replace this comment with your implementation
}

int Datastructures::stop_count()
{
    return stops.size();
}

void Datastructures::clear_all()
{
    stops.clear();
    regions.clear();
    stopNames.clear();
    maxCoordStop = NO_STOP;
    minCoordStop = NO_STOP;
}

std::vector<StopID> Datastructures::all_stops()
{
    std::vector<StopID> allstops = {};

    for(StopsDS::iterator it = stops.begin(); it != stops.end(); ++it){
        allstops.push_back(it->first);
    }

    return allstops;
}

bool Datastructures::add_stop(StopID id, const Name& name, Coord xy)
{
    // If the stop id already exists, return false
    auto search = stops.find(id);
    if (search != stops.end()){
        return false;
    }

    // Define stop's distance to zero
    float distance;
    distance = sqrt (pow( xy.x, 2) + pow(xy.y, 2));

    // Insert stop to data structure
    StopData stopinfo{name, xy, distance, NO_REGION};
    stops.insert(std::make_pair(id, stopinfo));

    // Check if stop's coordinates are min/max and update them if necessary
    if (minCoordStop == NO_STOP || distance < stops[minCoordStop].distanceFromZero){
        minCoordStop = id;
    }

    if (maxCoordStop == NO_STOP || distance > stops[maxCoordStop].distanceFromZero){
        maxCoordStop = id;
    }

    // Add stop name and id to stopNames ([]-method creates a new instance
    // if it doesn't exist yet)
    stopNames[name].push_back(id);

    return true;
    }

Name Datastructures::get_stop_name(StopID id)
{
    auto search = stops.find(id);

    // Error handling
    if (search == stops.end()){
        return NO_NAME;
    }

    return search->second.name;
}

Coord Datastructures::get_stop_coord(StopID id)
{
    auto search = stops.find(id);

    // Error handling
    if (search == stops.end()){
        return NO_COORD;
    }

    return search->second.coordinates;
}

std::vector<StopID> Datastructures::stops_alphabetically()
{
    // Error handling
    if (stops.empty()){
        return {NO_STOP};
    }

    std::vector<StopID> stopIDs;
    std::multimap<std::string, int> stopsAlphabetically;

    // Create a map containing names and IDs
    // Map maintains order automatically
    for (auto it = stops.begin(); it != stops.end(); ++it){
        stopsAlphabetically.insert(std::make_pair(it->second.name, it->first));
    }

    // Create a returnable vector of only IDs
    for (auto it = stopsAlphabetically.begin(); it != stopsAlphabetically.end(); ++it){
        stopIDs.push_back(it->second);
    }

    return stopIDs;
}

std::vector<StopID> Datastructures::stops_coord_order()
{
    // Error handling
    if (stops.empty()){
        return {NO_STOP};
    };

    std::vector<StopID> stopIDs;
    std::map<Coord, StopID> stopsCoordOrder;

    // Create a map containing coordinates and IDs
    // Map maintains coord order automatically
    for (auto it = stops.begin(); it != stops.end(); ++it){
        stopsCoordOrder.insert(std::make_pair(it->second.coordinates, it->first));
    }

    // Create a returnable vector of only IDs
    for (auto it = stopsCoordOrder.begin(); it != stopsCoordOrder.end(); ++it){
        stopIDs.push_back(it->second);
    }

    return stopIDs;
}

StopID Datastructures::min_coord()
{
    return minCoordStop;
}

StopID Datastructures::max_coord()
{
    return maxCoordStop;
}

std::vector<StopID> Datastructures::find_stops(Name const& name)
{
    auto search = stopNames.find(name);

    // Error handling
    if (search == stopNames.end()){
        return {};
    }

    return search->second;
}

bool Datastructures::change_stop_name(StopID id, const Name& newname)
{
    // Find the stop and simultaneously check that it exists
    auto search = stops.find(id);
    if (search == stops.end()){
        return false;
    }

    // Change the stop's name in the stopName-vector
    Name stopName = get_stop_name(id);

    // Remove the old one
    auto searchStopName = stopNames.find(stopName);
    std::vector<StopID>& namesIDs = searchStopName->second;
    namesIDs.erase(std::remove(namesIDs.begin(), namesIDs.end(), id), namesIDs.end());
    // If certain name doesn't have any IDs, delete the name
    if (namesIDs.empty()){
        stopNames.erase(stopName);
    }
    // Insert the ID under a new name
    stopNames[newname].push_back(id);

    // Change the name in the stops-datastructure
    search->second.name = newname;

    return true;
}

bool Datastructures::change_stop_coord(StopID id, Coord newcoord)
{
    // Find the stop and simultaneously check that it exists
    auto search = stops.find(id);
    if (search == stops.end()){
        return false;
    }

    // Update stops coordinates and distance to zero
    search->second.coordinates = newcoord;
    search->second.distanceFromZero = sqrt (pow( newcoord.x, 2) + pow(newcoord.y, 2));

    return true;
}

bool Datastructures::add_region(RegionID id, const Name &name)
{
    // Find if the region exists
    auto search = regions.find(id);
    if (search != regions.end()){
        return false;
    }

    // Insert region to data structure
    RegionData regioninfo{name, NO_REGION, {}};
    regions.insert(std::make_pair(id, regioninfo));

    return true;
}

Name Datastructures::get_region_name(RegionID id)
{  
    auto search = regions.find(id);

    // Error handling
    if (search == regions.end()){
        return NO_NAME;
    }

    return search->second.name;
}

std::vector<RegionID> Datastructures::all_regions()
{
    // Check if there is any regions
    if (regions.empty()){
        return {NO_REGION};
    }

    std::vector<RegionID> regionIDs;

    // Loop through regions and save the IDs to a returnable vector
    for (auto it = regions.begin(); it != regions.end(); ++it){
       regionIDs.push_back(it->first);
    }

    return regionIDs;
}

bool Datastructures::add_stop_to_region(StopID id, RegionID parentid)
{
    auto searchStop = stops.find(id);
    auto searchRegion = regions.find(parentid);

    // Check that stop and region both exist and that stop doesn't belong to a region yet
    if (searchStop == stops.end() ||
            searchRegion == regions.end() ||
            searchStop->second.region != NO_REGION){
        return false;
    }

    // Set the region to a stop
    searchStop->second.region = parentid;
    searchRegion->second.regionStops.push_back(id);
    return true;
}

bool Datastructures::add_subregion_to_region(RegionID id, RegionID parentid)
{
    auto searchSubRegion = regions.find(id);
    auto searchParentRegion = regions.find(parentid);

    // Check that both regions exist and that subregion doesn't have a parent yet
    if (searchSubRegion == regions.end() ||
            searchParentRegion == regions.end() ||
            searchSubRegion->second.parentRegion != NO_REGION){
        return false;
    }

    // Set the parent and child region to regions
    searchSubRegion->second.parentRegion = parentid;
    searchParentRegion->second.childRegions.push_back(id);

    // If parentregion has parentregion, add the subregion as their child as well
    while (searchParentRegion->second.parentRegion != NO_REGION){
        searchParentRegion = regions.find(searchParentRegion->second.parentRegion);
        searchParentRegion->second.childRegions.push_back(id);
    }


    return true;
}

std::vector<RegionID> Datastructures::stop_regions(StopID id)
{
    auto searchStop = stops.find(id);

    // Check that the stop exists
    if (searchStop == stops.end()){
        return {NO_REGION};
    }

    // If there is no region assigned to stop return empty vector
    RegionID region = searchStop->second.region;

    if (region == NO_REGION){
        return {};
    }

    // If pre-conditions are met, add region to the vector and search
    //if it has parent regions
    std::vector<RegionID> regionsIDs;
    regionsIDs.push_back(region);

    auto searchRegion = regions.find(region);
    RegionID parentRegion;

    while (searchRegion != regions.end()){
        parentRegion = searchRegion->second.parentRegion;
        if (parentRegion != NO_REGION){
            regionsIDs.push_back(parentRegion);
        }
        searchRegion = regions.find(parentRegion);
    }

    return regionsIDs;
}

void Datastructures::creation_finished()
{
    // Replace this comment with your implementation
    // You don't have to use this method for anything, if you don't need it
}

std::pair<Coord,Coord> Datastructures::region_bounding_box(RegionID id)
{
    // Find region
    auto searchRegion = regions.find(id);

    // Check if the region exists
    if (searchRegion == regions.end()){
        return {NO_COORD, NO_COORD};
    }

    Coord max_coord, min_coord;
    std::vector<StopID> regionsStops;
    std::vector<RegionID> searchRegions;
    bool stopsFound = false;

    // Find region's child regions and create a vector of regions to be included
    // in the bounding box
    searchRegions = searchRegion->second.childRegions;
    searchRegions.push_back(id);

    // Find all the stops of the regions included in the box
    std::vector<StopID> tempVector;
    for (auto it = searchRegions.begin(); it != searchRegions.end(); it++){
        tempVector = regions.find(*it)->second.regionStops;
        regionsStops.insert(regionsStops.end(), tempVector.begin(), tempVector.end());
    }

    // Go through all the stops coordinates and save min- and max-coordinates
    Coord tempCoord;
    for (auto it = regionsStops.begin(); it != regionsStops.end(); it++){
        tempCoord = get_stop_coord(*it);
        int x = tempCoord.x;
        int y = tempCoord.y;

        // If the stop is first to be found initialize the values, otherwise
        // compare them to existing max and min values
        if (!stopsFound){
            max_coord.x = x;
            min_coord.x = x;
            max_coord.y = y;
            min_coord.y = y;
        }
        else {
            if (x > max_coord.x){max_coord.x = x;}
            if (x < min_coord.x){min_coord.x = x;}
            if (y > max_coord.y){max_coord.y = y;}
            if (y < min_coord.y){min_coord.y = y;}
        }
        stopsFound = true;
    }

    if (!stopsFound){
        return {NO_COORD, NO_COORD};
    }

    return {min_coord, max_coord};
}

std::vector<StopID> Datastructures::stops_closest_to(StopID id)
{
    auto search = stops.find(id);

    // Check that the given stop exists
    if (search == stops.end()){
        return {NO_STOP};
    }

    std::vector<StopID> stopsClosestTo;
    std::map<float, StopID> stopsDistOrder;
    int x, y, comp_x, comp_y;
    float distance;

    x = search->second.coordinates.x;
    y = search->second.coordinates.y;

    // Go through all the stops and create a map containing stops' distances from
    // the original one and their id's. Map maintains the order automatically.
    for (auto it = stops.begin(); it != stops.end(); it++){
        if (it->first != id){
            comp_x = it->second.coordinates.x;
            comp_y = it->second.coordinates.y;
            distance = sqrt(pow(x - comp_x, 2) + pow(y - comp_y, 2));
            stopsDistOrder.insert(std::make_pair(distance, it->first));
        }
    }

    // Create a returnable vector of max 5 stops ID's
    int i = 0;
    for (auto it = stopsDistOrder.begin(); it!= stopsDistOrder.end() && i < 5; it++){
        stopsClosestTo.push_back(it->second);
        i++;
    }

    return stopsClosestTo;
}

bool Datastructures::remove_stop(StopID id)
{
    // Check if the given ID exists
    auto search = stops.find(id);

    // Error handling
    if (search == stops.end()){
        return false;
    }

    Name stopName = get_stop_name(id);

    // If ID exists, remove it from stops and update max and min
    // coordinate variables if necessary
    stops.erase(search);

    if (id == minCoordStop || id == maxCoordStop){
        std::vector<StopID> stopsCoordOrder = Datastructures::stops_coord_order();
        minCoordStop = stopsCoordOrder.front();
        maxCoordStop = stopsCoordOrder.back();
    }

    // Remove stop from the region's stop vector
    auto searchRegion = regions.find(search->second.region);
    std::vector<StopID>& regionStops = searchRegion->second.regionStops;
    regionStops.erase(std::remove(regionStops.begin(), regionStops.end(), id), regionStops.end());

    // Remove stop from the stopNames-datastructure's
    // stop vector
    auto searchStopName = stopNames.find(stopName);
    std::vector<StopID>& namesStops = searchStopName->second;
    namesStops.erase(std::remove(namesStops.begin(), namesStops.end(), id), namesStops.end());
    if (namesStops.empty()){
        stopNames.erase(stopName);
    }

    return true;
}

RegionID Datastructures::stops_common_region(StopID id1, StopID id2)
{
    // Error handling, checking id's existence is handled in the stop_regions-function
    std::vector<RegionID> stop1Regions = stop_regions(id1);
    std::vector<RegionID> stop2Regions = stop_regions(id2);

    // If no regions are found for either of stops
    if (stop1Regions.empty() ||
            stop2Regions.empty()){
        return NO_REGION;
    }

    // Find first common region of both stops
    auto result = std::find_first_of(stop1Regions.begin(),
                                     stop1Regions.end(),
                                     stop2Regions.begin(),
                                     stop2Regions.end());

    // If no common regions are found
    if (result == stop1Regions.end()){
        return NO_REGION;
    }

    return *result;
}

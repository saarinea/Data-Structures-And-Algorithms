// Datastructures.cc

#include "datastructures.hh"

#include <random>
#include <cmath>
#include <stdexcept>
#include <iostream>
#include <algorithm>
#include <functional>
#include <stack>

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
    return stops_.size();
}

void Datastructures::clear_all()
{
    // Phase 1
    stops_.clear();
    regions_.clear();
    stopNames_.clear();
    maxCoordStop_ = NO_STOP;
    minCoordStop_ = NO_STOP;

    // Phase 2
    clear_routes();
}

std::vector<StopID> Datastructures::all_stops()
{
    std::vector<StopID> allstops = {};

    for(StopsDS::iterator it = stops_.begin(); it != stops_.end(); ++it){
        allstops.push_back(it->first);
    }

    return allstops;
}

bool Datastructures::add_stop(StopID id, const Name& name, Coord xy)
{
    // If the stop id already exists, return false
    auto search = stops_.find(id);
    if (search != stops_.end()){
        return false;
    }

    // Define stop's distance to zero
    float distance;
    distance = sqrt (pow( xy.x, 2) + pow(xy.y, 2));

    // Insert stop to data structure
    StopData stopinfo{id, name, xy, distance};
    stops_.insert(std::make_pair(id, stopinfo));

    // Check if stop's coordinates are min/max and update them if necessary
    if (minCoordStop_ == NO_STOP || distance < stops_[minCoordStop_].distanceFromZero){
        minCoordStop_ = id;
    }

    if (maxCoordStop_ == NO_STOP || distance > stops_[maxCoordStop_].distanceFromZero){
        maxCoordStop_ = id;
    }

    // Add stop name and id to stopNames ([]-method creates a new instance
    // if it doesn't exist yet)
    stopNames_[name].push_back(id);

    return true;
}

Name Datastructures::get_stop_name(StopID id)
{
    auto search = stops_.find(id);

    // Error handling
    if (search == stops_.end()){
        return NO_NAME;
    }

    return search->second.name;
}

Coord Datastructures::get_stop_coord(StopID id)
{
    auto search = stops_.find(id);

    // Error handling
    if (search == stops_.end()){
        return NO_COORD;
    }

    return search->second.coordinates;
}

std::vector<StopID> Datastructures::stops_alphabetically()
{
    // Error handling
    if (stops_.empty()){
        return {NO_STOP};
    }

    std::vector<StopID> stopIDs;
    std::multimap<std::string, int> stopsAlphabetically;

    // Create a map containing names and IDs
    // Map maintains order automatically
    for (auto it = stops_.begin(); it != stops_.end(); ++it){
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
    if (stops_.empty()){
        return {NO_STOP};
    };

    std::vector<StopID> stopIDs;
    std::map<Coord, StopID> stopsCoordOrder;

    // Create a map containing coordinates and IDs
    // Map maintains coord order automatically
    for (auto it = stops_.begin(); it != stops_.end(); ++it){
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
    return minCoordStop_;
}

StopID Datastructures::max_coord()
{
    return maxCoordStop_;
}

std::vector<StopID> Datastructures::find_stops(Name const& name)
{
    auto search = stopNames_.find(name);

    // Error handling
    if (search == stopNames_.end()){
        return {};
    }

    return search->second;
}

bool Datastructures::change_stop_name(StopID id, const Name& newname)
{
    // Find the stop and simultaneously check that it exists
    auto search = stops_.find(id);
    if (search == stops_.end()){
        return false;
    }

    // Change the stop's name in the stopName-vector
    Name stopName = get_stop_name(id);

    // Remove the old one
    auto searchStopName = stopNames_.find(stopName);
    std::vector<StopID>& namesIDs = searchStopName->second;
    namesIDs.erase(std::remove(namesIDs.begin(), namesIDs.end(), id), namesIDs.end());
    // If certain name doesn't have any IDs, delete the name
    if (namesIDs.empty()){
        stopNames_.erase(stopName);
    }
    // Insert the ID under a new name
    stopNames_[newname].push_back(id);

    // Change the name in the stops-datastructure
    search->second.name = newname;

    return true;
}

bool Datastructures::change_stop_coord(StopID id, Coord newcoord)
{
    // Find the stop and simultaneously check that it exists
    auto search = stops_.find(id);
    if (search == stops_.end()){
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
    auto search = regions_.find(id);
    if (search != regions_.end()){
        return false;
    }

    // Insert region to data structure
    RegionData regioninfo{name, NO_REGION, {}};
    regions_.insert(std::make_pair(id, regioninfo));

    return true;
}

Name Datastructures::get_region_name(RegionID id)
{
    auto search = regions_.find(id);

    // Error handling
    if (search == regions_.end()){
        return NO_NAME;
    }

    return search->second.name;
}

std::vector<RegionID> Datastructures::all_regions()
{
    // Check if there is any regions
    if (regions_.empty()){
        return {NO_REGION};
    }

    std::vector<RegionID> regionIDs;

    // Loop through regions and save the IDs to a returnable vector
    for (auto it = regions_.begin(); it != regions_.end(); ++it){
       regionIDs.push_back(it->first);
    }

    return regionIDs;
}

bool Datastructures::add_stop_to_region(StopID id, RegionID parentid)
{
    auto searchStop = stops_.find(id);
    auto searchRegion = regions_.find(parentid);

    // Check that stop and region both exist and that stop doesn't belong to a region yet
    if (searchStop == stops_.end() ||
            searchRegion == regions_.end() ||
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
    auto searchSubRegion = regions_.find(id);
    auto searchParentRegion = regions_.find(parentid);

    // Check that both regions exist and that subregion doesn't have a parent yet
    if (searchSubRegion == regions_.end() ||
            searchParentRegion == regions_.end() ||
            searchSubRegion->second.parentRegion != NO_REGION){
        return false;
    }

    // Set the parent and child region to regions
    searchSubRegion->second.parentRegion = parentid;
    searchParentRegion->second.childRegions.push_back(id);

    // If parentregion has parentregion, add the subregion as their child as well
    while (searchParentRegion->second.parentRegion != NO_REGION){
        searchParentRegion = regions_.find(searchParentRegion->second.parentRegion);
        searchParentRegion->second.childRegions.push_back(id);
    }


    return true;
}

std::vector<RegionID> Datastructures::stop_regions(StopID id)
{
    auto searchStop = stops_.find(id);

    // Check that the stop exists
    if (searchStop == stops_.end()){
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

    auto searchRegion = regions_.find(region);
    RegionID parentRegion;

    while (searchRegion != regions_.end()){
        parentRegion = searchRegion->second.parentRegion;
        if (parentRegion != NO_REGION){
            regionsIDs.push_back(parentRegion);
        }
        searchRegion = regions_.find(parentRegion);
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
    auto searchRegion = regions_.find(id);

    // Check if the region exists
    if (searchRegion == regions_.end()){
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
        tempVector = regions_.find(*it)->second.regionStops;
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
    auto search = stops_.find(id);

    // Check that the given stop exists
    if (search == stops_.end()){
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
    for (auto it = stops_.begin(); it != stops_.end(); it++){
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
    auto search = stops_.find(id);

    // Error handling
    if (search == stops_.end()){
        return false;
    }

    Name stopName = get_stop_name(id);

    // If ID exists, remove it from stops and update max and min
    // coordinate variables if necessary
    stops_.erase(search);

    if (id == minCoordStop_ || id == maxCoordStop_){
        std::vector<StopID> stopsCoordOrder = Datastructures::stops_coord_order();
        minCoordStop_ = stopsCoordOrder.front();
        maxCoordStop_ = stopsCoordOrder.back();
    }

    // Remove stop from the region's stop vector
    auto searchRegion = regions_.find(search->second.region);
    std::vector<StopID>& regionStops = searchRegion->second.regionStops;
    regionStops.erase(std::remove(regionStops.begin(), regionStops.end(), id), regionStops.end());

    // Remove stop from the stopNames-datastructure's
    // stop vector
    auto searchStopName = stopNames_.find(stopName);
    std::vector<StopID>& namesStops = searchStopName->second;
    namesStops.erase(std::remove(namesStops.begin(), namesStops.end(), id), namesStops.end());
    if (namesStops.empty()){
        stopNames_.erase(stopName);
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

std::vector<RouteID> Datastructures::all_routes()
{
    std::vector<RouteID> allRoutes = {};

    for(RoutesDS::iterator it = routes_.begin(); it != routes_.end(); ++it){
        allRoutes.push_back(it->first);
    }

    return allRoutes;
}

bool Datastructures::add_route(RouteID id, std::vector<StopID> stops)
{     
    // Error handling: if the route id already exists, return false
    auto search = routes_.find(id);
    if (search != routes_.end()){
        return false;
    }

    // Error handling: If there is only one stop ID, return false
    if (stops.size() <= 1) {
        return false;
    }

    // Error handling: If some of the stop ID's do not exist, return false
    // Simultaneously save the stops' pointers to a vector to be used later
    std::vector<StopData *> stopPtrVect = {};
    for (auto it = stops.begin(); it != stops.end(); it++){
        auto search = stops_.find(*it);
        if (search == stops_.end()){
            return false;
        }
        stopPtrVect.push_back(&search->second);
    }

    // Insert route to data structure
    RouteData routedata{stops, stopPtrVect};
    routes_.insert(std::make_pair(id, routedata));

    // Insert route to its stops
    for (auto it = stopPtrVect.begin(); std::next(it) != stopPtrVect.end(); it++){

        auto from = *it;
        auto to = *std::next(it);

        Coord c1 = from->coordinates;
        Coord c2 = to->coordinates;

        int distance = static_cast<int> (sqrt (pow ( c1.x - c2.x, 2) + pow ( c1.y - c2.y, 2)));

        RouteEdge newEdge{id, to, distance};
        from->routesTo.insert(std::make_pair(id, newEdge));
    }

    return true;
}

std::vector<std::pair<RouteID, StopID>> Datastructures::routes_from(StopID stopid)
{
    // Error handling: if the stop doesn't exist
    auto search = stops_.find(stopid);
    if (search == stops_.end()){
        return {{NO_ROUTE, NO_STOP}};
    }

    // Get the routes (edges) starting from the stop and create a returnable vector
    std::vector<std::pair<RouteID, StopID>> returnableEdges = {};
    const auto& stopEdges = search->second.routesTo;

    for (auto it = stopEdges.begin(); it != stopEdges.end(); it++){
        returnableEdges.push_back(std::make_pair(it->second.route, it->second.nextStop->id));
    }

    return returnableEdges;
}

std::vector<StopID> Datastructures::route_stops(RouteID id)
{
    // Error handling: if the stop doesn't exist
    auto search = routes_.find(id);
    if (search == routes_.end()){
        return {NO_STOP};
    }

    // Return stops from the searched route
    return search->second.stops;
}

void Datastructures::clear_routes()
{
    // Clear the datastructure for routes
    routes_.clear();

    // Clear routes and trips from each individual stop (StopsDS stops_-datastructure)
    for (auto it = stops_.begin(); it != stops_.end(); it ++){
        it->second.routesTo.clear();
    }
}

std::vector<std::tuple<StopID, RouteID, Distance>> Datastructures::journey_any(StopID fromstop, StopID tostop)
{
    // journey_least_stops was chosen because it's more efficient than journey_earliest_arrival
    return journey_least_stops(fromstop, tostop);
}

std::vector<std::tuple<StopID, RouteID, Distance>> Datastructures::journey_least_stops(StopID fromstop, StopID tostop)
{
    // Error handling: check that both stops exist
    auto searchFrom = stops_.find(fromstop);
    auto searchTo = stops_.find(tostop);
    auto stopsEnd = stops_.end();

    if (searchFrom == stopsEnd || searchTo == stopsEnd){
        return {{NO_STOP, NO_ROUTE, NO_DISTANCE}};
    }

    //BFS Algorithm:
    // Initialize source node
    StopData * s = &searchFrom->second;

    // Declare the queue
    std::queue<StopData *> q;

    // Initialize all nodes
    for (auto it = stops_.begin(); it != stops_.end(); it++){
        it->second.colour = WHITE;
        it->second.d = INF;
        it->second.pii = NULL;
        it->second.dRoute = NO_ROUTE;
    }

    // Initialize source node settings
    s->colour = GREY;
    s->d = 0;
    q.push(s);

    while(!q.empty()){
        StopData * u = q.front();
        q.pop();

        // Adjacency list is a unordered_map of Edges, iterator it represents a single Edge
        const auto &adj = u->routesTo;
        for (auto it = adj.begin(); it != adj.end(); it++){

            auto v = it->second.nextStop;

            int distance = it->second.distance;
            RouteID route = it->second.route;

            if (v->colour == WHITE){
                v->colour = GREY;
                v->d = u->d + distance;
                v->pii = u;
                v->dRoute = route;
                q.push(v);
            }
        }

        u->colour = BLACK;
    }

    // Travel through the created route of pii's and add to returnable vector
    std::vector<std::tuple<StopID, RouteID, Distance>> returnableStops;
    std::tuple<StopID, RouteID, Distance> stop;

    auto iter = &searchTo->second;
    RouteID routeTemp = NO_ROUTE; // A helper variable to "move" the route id to the right place

    while(true){
        stop = std::make_tuple(iter->id, routeTemp, iter->d);
        returnableStops.push_back(stop);
        if(iter->pii == NULL){break;}
        routeTemp = iter->dRoute;
        iter = iter->pii;
    }

    // If the route was not found, return empty vector
    if (iter != &searchFrom->second) {
        return {};
    }

    // Correct the order
    std::reverse(returnableStops.begin(), returnableStops.end());

    return returnableStops;
}

std::vector<std::tuple<StopID, RouteID, Distance>> Datastructures::journey_with_cycle(StopID fromstop)
{
    // Error handling: check that the stop exists
    auto search = stops_.find(fromstop);
    if (search == stops_.end()){
        return {{NO_STOP, NO_ROUTE, NO_DISTANCE}};
    }

    // DFS algorithm:
    // Initialize source node
    StopData * s = &search->second;

    // Declare the stack
    std::stack<StopData *> stack;

    // Initialize all nodes
    for (auto it = stops_.begin(); it != stops_.end(); it++){
        it->second.colour = WHITE;
        it->second.d = INF;
        it->second.pii = NULL;
        it->second.dRoute = NO_ROUTE;
    }

    // Initialize source node settings
    s->d = 0;
    stack.push(s);

    // Initialize variables to save the final node of the sycle
    bool cycleFound = false;
    Distance cycleLastDistance = NO_DISTANCE;
    RouteID cycleLastRoute = NO_ROUTE;
    StopData * cycleLastPii = NULL;
    StopID cycleLastId = NO_VALUE;

    while (!stack.empty()) {
        StopData * u = stack.top();
        stack.pop();

        if (u->colour == WHITE){
            u->colour = GREY;
            stack.push(u);

            const auto &adj = u->routesTo;
            for (auto it = adj.begin(); it!= adj.end(); it++){

                // Walking routes are not taken into account in this function,
                // wouldn't make sense
                if (it->second.walking) {
                    continue;
                }

                auto v = it->second.nextStop;

                int distance = it->second.distance;
                RouteID route = it->second.route;

                // Unvisited node
                if (v->colour == WHITE){
                    v->d = u->d + distance;
                    v->pii = u;
                    v->dRoute = route;
                    stack.push(v);
                }
                // Once visited node -> cycle -> save necessary information and break
                else if (v->colour == GREY) {
                    cycleLastDistance = u->d + distance;
                    cycleLastPii = u;
                    cycleLastRoute = route;
                    cycleLastId = v->id;
                    cycleFound = true;
                    break;
                }
            }

            if (cycleFound){
                break;
            }
        }
        else {
            u->colour = BLACK;
        }
    }

    // If there is no cycle found return empty vector
    if (!cycleFound) {
        return {};
    }

    // Travel through the created route of pii's (starting from the second last node
    // in the cycle) and add to returnable vector
    std::vector<std::tuple<StopID, RouteID, Distance>> returnableStops = {};
    std::tuple<StopID, RouteID, Distance> stop;

    auto iter = cycleLastPii;
    RouteID routeTemp = cycleLastRoute; // A helper variable to "move" the route id to the right place

    while(true){
        stop = std::make_tuple(iter->id, routeTemp, iter->d);
        returnableStops.push_back(stop);
        if(iter->pii == NULL){break;} // Arrive to the starting node (or other which leads nowhere)
        routeTemp = iter->dRoute;
        iter = iter->pii;
    }

    // If the route to starting node was not found, return empty vector
    if (iter->id != fromstop) {
        return {};
    }

    // Correct the order
    std::reverse(returnableStops.begin(), returnableStops.end());

    // Add the final node to vector's end
    returnableStops.push_back(std::make_tuple(cycleLastId, NO_ROUTE, cycleLastDistance));

    return returnableStops;
}

std::vector<std::tuple<StopID, RouteID, Distance>> Datastructures::journey_shortest_distance(StopID fromstop, StopID tostop)
{
    // Error handling: check that both stops exist
    auto searchFrom = stops_.find(fromstop);
    auto searchTo = stops_.find(tostop);
    auto stopsEnd = stops_.end();

    if (searchFrom == stopsEnd || searchTo == stopsEnd){
        return {{NO_STOP, NO_ROUTE, NO_DISTANCE}};
    }

    // A*-algorithm:
    // Initialize source and goal node (stop when goal node is found)
    StopData * s = &searchFrom->second;
    StopData * goal = &searchTo->second;
    bool goalFound = false;

    // Initialize goal coordinates
    int goal_x = goal->coordinates.x;
    int goal_y = goal->coordinates.y;

    // Declare the priority queue
    std::priority_queue<PqPair, std::deque<PqPair>, std::greater<PqPair>> pq;

    // Initialize all nodes
    for (auto it = stops_.begin(); it != stops_.end(); it++){
        it->second.colour = WHITE;
        it->second.d = INF;
        it->second.de = INF;
        it->second.pii = NULL;
        it->second.dRoute = NO_ROUTE;
    }

    // Initialize source node settings
    s->colour = GREY;
    s->d = 0;
    s->de = 0;
    pq.push(std::make_pair(s->de, s));

    while(!pq.empty()){

        // Extract-min done with operations top() and pop()
        StopData * u = pq.top().second;
        pq.pop();

        // If goal stop is found, stop the algorithm
        if (u == goal){
            goalFound = true;
            break;
        }

        // Adjacency list is a vector of Edges
        const auto &adj = u->routesTo;
        for (auto it = adj.begin(); it != adj.end(); it++){

            auto v = it->second.nextStop;

            int distance = it->second.distance;
            RouteID route = it->second.route;

            if (v->colour == WHITE){
                v->colour = GREY;
                pq.push(std::make_pair(v->de, v));
            }

            // Relaxation, if the shorter path is found
            if (v->d > u->d + distance){
                v->d = u->d + distance;
                int comp_x = v->coordinates.x;
                int comp_y = v->coordinates.y;
                int min_est = sqrt(pow(goal_x - comp_x, 2) + pow(goal_y - comp_y, 2));
                v->de = v->d + min_est;
                v->pii = u;
                v->dRoute = route;
                pq.push(std::make_pair(v->de, v));
            }
        }
        u->colour = BLACK;

    } // End of A*

    // If the goal node/stop was not found, return an empty vector
    if (!goalFound) {
        return {};
    }

    // Travel through the created route of pii's and add to returnable vector
    std::vector<std::tuple<StopID, RouteID, Distance>> returnableStops;
    std::tuple<StopID, RouteID, Distance> stop;

    auto iter = &searchTo->second;
    RouteID routeTemp = NO_ROUTE; // A helper variable to "move" the route id to the right place

    while(true){
        stop = std::make_tuple(iter->id, routeTemp, iter->d);
        returnableStops.push_back(stop);
        if(iter->pii == NULL){break;}
        routeTemp = iter->dRoute;
        iter = iter->pii;
    }

    // If the route from was not found, return empty vector
    // This is checked twice (if !goalFound above), because when the route exists
    // but the trip doesn't, Dijkstra reaches the goal node but the arrival time is INF
    if (iter != &searchFrom->second) {
        return {};
    }

    // Correct the order
    std::reverse(returnableStops.begin(), returnableStops.end());

    return returnableStops;
}

bool Datastructures::add_trip(RouteID routeid, std::vector<Time> const& stop_times)
{
    // Error handling: check that the route exists
    auto search = routes_.find(routeid);
    if (search == routes_.end()){
        return false;
    }

    // Add trip to route's data structure
    search->second.trips.push_back(stop_times);

    // Add trip's times to route's stops
    std::vector<StopData *> routesStops = search->second.stopPtrs;
    int i = 0;

    // Initialize values
    Time arrivalTime = NO_TIME;
    Time nextArrivalTime = NO_TIME;

    // Add trip's data to each stop's trip-vector
    for (auto it = routesStops.begin(); std::next(it) != routesStops.end(); it++){
        arrivalTime = stop_times[i];
        nextArrivalTime = stop_times[i+1];
        TripEdge tripEdge{arrivalTime, nextArrivalTime};
        auto & stop = *it;
        stop->routesTo.find(routeid)->second.trips.push_back(tripEdge);
        i += 1;
    }

    return true;
}

std::vector<std::pair<Time, Duration>> Datastructures::route_times_from(RouteID routeid, StopID stopid)
{
    // Error handling: check that the route and stop exists
    auto searchRoute = routes_.find(routeid);
    auto searchStop = stops_.find(stopid);

    if (searchRoute == routes_.end() || searchStop == stops_.end()){
        return {{NO_TIME, NO_DURATION}};
    }

    const std::unordered_multimap<RouteID, RouteEdge> &routesFromStop = searchStop->second.routesTo;

    // Error handling: check that the stop is in given route i.e.
    // there is a given route leaving from the stop
    auto searchRoute2 = routesFromStop.find(routeid);

    if (searchRoute2 == routesFromStop.end()) {
        return {{NO_TIME, NO_DURATION}};
    }

    RouteData routeData = routes_.find(routeid)->second;

    // Initial values
    Time time = NO_VALUE;
    Duration duration = NO_VALUE;
    const std::vector<std::vector<Time>> &trips = routeData.trips;
    int stopIndex = NO_STOP;
    std::vector<std::pair<Time, Duration>> returnableVector = {};

    std::vector<StopID> routesStops = routeData.stops;

    // Search the place of stop in trip time vector
    for (auto i = 0; i != int(routesStops.size()); i++){
        if (routesStops[i] == stopid){
            stopIndex = i;
            break;
        }
    }

    // Loop through the trips and add times to the returnable vector
    for (auto tripIt = trips.begin(); tripIt != trips.end(); tripIt++){
        const std::vector<Time> &singleTripTimes = *tripIt;
        time = singleTripTimes[stopIndex];
        duration = singleTripTimes[stopIndex + 1] - time; // over indexing not possible because the route
                                                          // has to be found from the stop earlier in the code
        returnableVector.push_back(std::make_pair(time, duration));
    }

    return returnableVector;
}

std::vector<std::tuple<StopID, RouteID, Time>> Datastructures::journey_earliest_arrival(StopID fromstop, StopID tostop, Time starttime)
{
    // Error handling: check that both stops exist
    auto searchFrom = stops_.find(fromstop);
    auto searchTo = stops_.find(tostop);
    auto stopsEnd = stops_.end();

    if (searchFrom == stopsEnd || searchTo == stopsEnd){
        return {{NO_STOP, NO_ROUTE, NO_DISTANCE}};
    }

    // Modified Dijkstra's algorithm with arrival time as a priority factor
    // Initialize source and goal node
    StopData * s = &searchFrom->second;
    StopData * goal = &searchTo->second;
    bool goalFound = false;

    // Declare the priority queue
    std::priority_queue<PqPair, std::deque<PqPair>, std::greater<PqPair>> pq;

    // Initialize all nodes
    for (auto it = stops_.begin(); it != stops_.end(); it++){
        it->second.colour = WHITE;
        it->second.d = INF;
        it->second.pii = NULL;
        it->second.dRoute = NO_ROUTE;
        it->second.departureFromPrevious = INF;
        it->second.arrivalTime = INF;
    }

    // Initialize source node settings
    s->colour = GREY;
    s->d = 0;
    s->arrivalTime = starttime;

    pq.push(std::make_pair(s->arrivalTime, s));

    while(!pq.empty()){

        // Extract-min done with operations top() and pop()
        StopData* u = pq.top().second;
        pq.pop();

        // If goal stop is found, stop the algorithm
        if (u == goal){
            goalFound = true;
            break;
        }

        // Adjacency list is a vector of RouteEdges
        const std::unordered_multimap<RouteID, RouteEdge> &adjNodes = u->routesTo;

        // Go through adjacent nodes i.e. possible routes
        for (auto it = adjNodes.begin(); it != adjNodes.end(); it++){

            auto v = it->second.nextStop;
            const std::vector<TripEdge> &trips = it->second.trips;

            Time optimalNextArrivalTime = INF;
            Time optimalDepartureTime = INF;
            RouteID route = NO_ROUTE;

            // Go through trips to a certain node
            for (auto iter = trips.begin(); iter != trips.end(); iter++){

                Time nextArrivalTime;
                Time departureTime;

                // If the connection is a walking connection, calculate the times
                // dynamically based on the previous stop's times
                if (it->second.walking) {
                    departureTime = u->arrivalTime;
                    nextArrivalTime = u->arrivalTime + it->second.distance / 1.39; // 5km/h
                }

                else {
                    nextArrivalTime = iter->nextArrivalTime;
                    departureTime = iter->departureTime;
                }

                // Check that trip's departure time (arrival time) is on or after arrival time
                if (departureTime < u->arrivalTime){
                    continue;
                }

                // If arrival time to next stop is earlier than earlier trips, update it
                if (nextArrivalTime < optimalNextArrivalTime){
                    optimalNextArrivalTime = nextArrivalTime;
                    optimalDepartureTime = departureTime;
                    route = it->second.route;
                }
            }

            // If stop is met for the first time push to priority queue
            if (v->colour == WHITE) {
                v->colour = GREY;
                pq.push(std::make_pair(v->arrivalTime, v));
            }

            // Relaxation, if the shorter path is found
            if (v->arrivalTime > optimalNextArrivalTime){
                v->arrivalTime = optimalNextArrivalTime;
                v->departureFromPrevious = optimalDepartureTime;
                v->pii = u;
                v->dRoute = route;
                pq.push(std::make_pair(v->arrivalTime, v));
            }
        }
        u->colour = BLACK;
    } // End of Dijkstra

    // If the goal stop was not found, return empty vector
    if (!goalFound) {
        return {};
    }

    // Travel through the created route of pii's and add variables to returnable vector
    std::vector<std::tuple<StopID, RouteID, Time>> returnableStops;
    std::tuple<StopID, RouteID, Distance> stop;

    // Helper variables to "move" the data to the right place
    RouteID routeTemp = NO_ROUTE;
    Time timeTemp = NO_VALUE;

    auto iter = &searchTo->second;

    while(true){
        // If the node is the final stop return arrival time, otherwise the departure time
        if (iter == &searchTo->second) {
            timeTemp = iter->arrivalTime;
        }

        stop = std::make_tuple(iter->id, routeTemp, timeTemp);
        returnableStops.push_back(stop);
        if(iter->pii == NULL){break;} // Arriving to first stop
        routeTemp = iter->dRoute;
        timeTemp = iter->departureFromPrevious;
        iter = iter->pii;
    }

    // If the route from was not found, return empty vector
    // This is checked twice (if !goalFound above), because when the route exists
    // but the trip doesn't, Dijkstra reaches the goal node but the arrival time is INF
    if (iter != &searchFrom->second) {
        return {};
    }

    // Correct the order
    std::reverse(returnableStops.begin(), returnableStops.end());

    return returnableStops;
}

void Datastructures::add_walking_connections()
{
    // Walking connections are added as a mockup routes. They are added to
    // all the stops, but they are not added to the routes_-datastructure. This
    // way they are not visible in the wrong functions / their return values, but
    // they can be found in different path algorithms (journey_earliest_arrival,
    // journey_shortest_distance, etc.). They are not used in journey_with_cycle
    // since a cycle would always be found.

    StopID stopId = NO_VALUE;
    StopData * stopData = NULL;
    StopData * arrivalStopData = NULL;

    // Loop through all stops
    for (auto it = stops_.begin(); it!= stops_.end(); it++) {

        stopId = it->first;
        stopData = &it->second;

        int x = it->second.coordinates.x;
        int y = it->second.coordinates.y;

        // Get closest 5 stops
        std::vector<StopID> closestStops = stops_closest_to(stopId);

        // Loop through those 5 closest stops
        for (auto stopIter = closestStops.begin(); stopIter != closestStops.end(); stopIter++){

            arrivalStopData = &stops_.find(*stopIter)->second;

            int comp_x = arrivalStopData->coordinates.x;
            int comp_y = arrivalStopData->coordinates.y;

            Distance distance = sqrt(pow(x - comp_x, 2) + pow(y - comp_y, 2));

            // Create a new routeEdge and TripEdge with walking parameters and
            // push it to the stop's routes
            TripEdge tripEdge{INF, INF};

            RouteEdge routeEdge{NO_ROUTE, arrivalStopData, distance, {}, true};
            routeEdge.trips.push_back(tripEdge);

            stopData->routesTo.insert(std::make_pair(NO_ROUTE, routeEdge));
        }
    }
}

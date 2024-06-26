#include <iostream>
#include <vector>
#include <algorithm>
#include <random>
#include<fstream>
#include<string>
#include<omp.h>

using namespace std;

// Define the number of cities
const int NUM_CITIES = 100;

// Define the maximum generations for the GA
const int MAX_GENERATIONS = 3000;

// Define the population size for the GA
const int POPULATION_SIZE = 100;

// Define the mutation rate for the GA
const double MUTATION_RATE = 0.0062;

// Define a structure to represent a city
struct City {
    int x;
    int y;
    City(int x, int y) : x(x), y(y) {}
};

// Define a structure to represent a route
struct Route {
    vector<int> path;
    double fitness;
};

// Calculate the distance between two cities
double calculateDistance(const City& city1, const City& city2) {
    int dx = city1.x - city2.x;
    int dy = city1.y - city2.y;
    return sqrt(dx * dx + dy * dy);
}

// Generate a random route
Route generateRandomRoute() {
    Route route;
    for (int i = 0; i < NUM_CITIES; ++i) {
        route.path.push_back(i);
    }
    random_shuffle(route.path.begin(), route.path.end());
    return route;
}

// Calculate the fitness of a route (smaller distance is better)
void calculateFitness(Route& route, const vector<City>& cities) {
    double totalDistance = 0.0;
    for (int i = 0; i < NUM_CITIES - 1; ++i) {
        int cityIndex1 = route.path[i];
        int cityIndex2 = route.path[i + 1];
        totalDistance += calculateDistance(cities[cityIndex1], cities[cityIndex2]);
    }
    // Add distance from last city back to the starting city
    int lastCityIndex = route.path[NUM_CITIES - 1];
    totalDistance += calculateDistance(cities[lastCityIndex], cities[route.path[0]]);
    route.fitness = totalDistance;
}

// Perform crossover between two parent routes to produce a child route
Route crossover(const Route& parent1, const Route& parent2) {
    Route child;
    int startPos = rand() % NUM_CITIES;
    int endPos = rand() % NUM_CITIES;

    for (int i = 0; i < NUM_CITIES; ++i) {
        if (startPos < endPos && i > startPos && i < endPos) {
            child.path.push_back(parent1.path[i]);
        }
        else if (startPos > endPos && !(i < startPos && i > endPos)) {
            child.path.push_back(parent1.path[i]);
        }
        else {
            child.path.push_back(-1);
        }
    }

    for (int i = 0; i < NUM_CITIES; ++i) {
        if (find(child.path.begin(), child.path.end(), parent2.path[i]) == child.path.end()) {
            for (int j = 0; j < NUM_CITIES; ++j) {
                if (child.path[j] == -1) {
                    child.path[j] = parent2.path[i];
                    break;
                }
            }
        }
    }

    return child;
}

// Mutate a route by swapping two cities
void mutate(Route& route) {
    for (int i = 0; i < NUM_CITIES; ++i) {
        if ((double)rand() / RAND_MAX < MUTATION_RATE) {
            int swapIndex = rand() % NUM_CITIES;
            swap(route.path[i], route.path[swapIndex]);
        }
    }
}

// Find the best route in a population
Route findBestRoute(const vector<Route>& population) {
    double bestFitness = numeric_limits<double>::max();
    int bestIndex = -1;
    for (int i = 0; i < POPULATION_SIZE; ++i) {
        if (population[i].fitness < bestFitness) {
            bestFitness = population[i].fitness;
            bestIndex = i;
        }
    }
    return population[bestIndex];
}

vector<City> initializeCities(string filename) {
    ifstream file;
    file.open("100.txt", ios::in);
    if (!file.is_open()) {
        cerr << "Error opening file: " << filename << endl;
        exit(1);
    }

    string numCities;
    file >> numCities;
    cout << numCities;
    vector<City> cities;
    int index, x, y;
    string s_index, s_x, s_y;
    for (int i = 0; i < 100; i++) {
        file >> s_index >> s_x >> s_y;
        index = std::stoi(s_index);
        x = std::stoi(s_x);
        y = std::stoi(s_y);

        cities.push_back(City(x, y));
    }

    file.close();
    return cities;
}

int main() {
    // Define the cities
    string filename = "100.txt";
    vector<City> cities = initializeCities(filename);

    // Initialize the population
    vector<Route> population;
    for (int i = 0; i < POPULATION_SIZE; ++i) {
        population.push_back(generateRandomRoute());
        calculateFitness(population[i], cities);
    }

    // Perform the GA iterations
    for (int generation = 0; generation < MAX_GENERATIONS; ++generation) {
        vector<Route> newPopulation;

        // Generate offspring through selection, crossover, and mutation
#pragma omp parallel for schedule(dynamic)
for (int i = 0; i < POPULATION_SIZE; ++i) {
    Route parent1 = findBestRoute(population);
    Route parent2 = findBestRoute(population);
    Route child = crossover(parent1, parent2);
    mutate(child);
    calculateFitness(child, cities);
#pragma omp critical
    newPopulation.push_back(child);
}

        // Replace the old population with the new population
        population = newPopulation;
    }

    // Find the best route
    Route bestRoute = findBestRoute(population);

    // Print the best route
    cout << "Best Route: ";
    for (int i = 0; i < NUM_CITIES; ++i) {
        cout << bestRoute.path[i] << " ";
    }
    cout << endl;

    // Print the total distance of the best route
    cout << "Total Distance: " << bestRoute.fitness << endl;

    return 0;
}

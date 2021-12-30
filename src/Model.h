#ifndef SEAIHCR_MODEL_H
#define SEAIHCR_MODEL_H

#include <vector>
#include <memory>
#include <stack>
#include "Compartment.h"
#include "muParser.h"

class Model {
private:
    /**
     * Contains all compartments of this model
     */
    std::vector<std::shared_ptr<Compartment>> comps;

    std::vector<std::string> paramNames;
    std::vector<double> paramValues;

    // Extract these values from comps vector
    std::vector<std::string> allCompNames;
    std::vector<double> allCompValues;

    /**
     * Contains weak pointers to all models, including itself, with corresponding contact probability stored at the
     * same indices in vector linkedContactRates
     */
    std::vector<std::weak_ptr<Model>> linkedModels;

    /**
     * Contain the contact probabilities that corresponds to the same indices in linkedModels
     */
    std::vector<double> linkedContactRates;
public:
    // Model structure and infectious compartment are the same for all models for a disease
    std::vector<std::string> transitions;
    Model(std::vector<std::string>& paramNames, std::vector<double>& paramValues);
    ~Model() {
//        std::cout << "Model destructor called." << std::endl;
    }
    std::vector<std::shared_ptr<Compartment>> getComps();

    /**
     * Compartments of a Model object are stored as pointer in vector <b>comps</b>, but we normally identify compartment
     * by name, this function take the compartment name as input and return the address of that compartment pointer
     * @param compName: a string that matches name of the compartment of interest
     * @return a weak pointer of that compartment
     */
    std::weak_ptr<Compartment> getAddressFromName(std::string compName);

    // Add compartment to model using JSON config file
    void addCompsFromConfig(std::vector<std::shared_ptr<Compartment>>& comps);

    /**
     * Return the index of a compartment in vector <b>comps</b>
     * @param comp: shared pointer of the compartment of interest
     * @return the index of that compartment
     */
    int getIndex(std::shared_ptr<Compartment> comp);

    // Functions to check cycle and sort compartments to the correct order
    // Use depth-first-search algorithm to detect cycle https://www.geeksforgeeks.org/detect-cycle-in-a-graph/
    bool checkCycleHelper(size_t i, std::vector<bool>& visited, std::vector<bool>& recursiveStack);
    void checkCycle();
    // Then we sort the compartments using topological sorting algorithm https://www.geeksforgeeks.org/topological-sorting/
    void sortCompsHelper(size_t i, std::vector<bool>& visited, std::stack<std::shared_ptr<Compartment>>& stack);
    /**
     * <b>sortComps()</b> comprises of many helper functions (<b>checkCycleHelper()</b>, <b>checkCycle()</b> and
     * <b>sortCompsHelper()</b>), it will first make sure that there is no cycle in the model, then perform topological
     * sorting algorithm
     */
    void sortComps();

    /**
     * Update subCompartments and total for each compartments in the model
     * @param iter
     */
    void update(size_t iter);

    /**
     * Get all compartment names from the comps vector and also initial values of allCompValues
     */
    void initAllComps();

    /**
     * Helper function to update allCompValues after each iteration
     */
    void updateAllCompValues(size_t iter);
};


#endif //SEAIHCR_MODEL_H

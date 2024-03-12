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

    std::vector<std::string> compsOrder;

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

    void sortCompsByInputTransition();

    /**
     * Update subCompartments and total for each compartments in the model
     * @param iter
     */
    void update(size_t iter);

    void addCompsOrder(std::string compOrder);
    std::vector<std::string> getCompsOrder(){
        return compsOrder;
    };

};


#endif //SEAIHCR_MODEL_H

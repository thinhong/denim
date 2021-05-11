#ifndef MAIN_CPP_FULLMODEL_H
#define MAIN_CPP_FULLMODEL_H

#include <vector>
#include <memory>
#include "Model.h"
#include "Contact.h"

class FullModel {
private:
    std::vector<std::shared_ptr<Model>> models;
    std::vector<std::shared_ptr<Contact>> allContacts;
public:
    explicit FullModel(std::vector<std::shared_ptr<Contact>> allContacts);
    FullModel() = default;
    void addModel(std::shared_ptr<Model> model);

    // Helper functions
    std::vector<std::shared_ptr<Model>> getModels();
    /**
     * Get models that contain 1 or more specific groups of interest
     * @param groupToGet: a vector contains the group names
     * @return a vector of weak pointers of models
     */
    std::vector<std::weak_ptr<Model>> getModelsWithSpecificGroup(std::vector<std::string> groupToGet);

    /**
     *
     * @param modelGroup
     * @return
     */
    double getContactRateByComparingPairs(std::vector<std::string> modelGroup);

    void connectModels();
};


#endif //MAIN_CPP_FULLMODEL_H

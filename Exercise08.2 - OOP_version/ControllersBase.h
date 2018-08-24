/// FILE NAME:      ControllersBase.h
/// AUTHOR:         Mihkel Tiganik
/// CREATED:        02.08.2018
/// MODIFIED:       02.08.2018

#ifndef CONTROLLERSBASE_H
#define CONTROLLERSBASE_H

#include "States.h"

// base methods for MVC controllers for override
class ControllersBase{

    public:
        virtual States onLeftButtonPress() = 0;
        virtual States onRightButtonPress() = 0;
        virtual void onUpButtonPress() = 0;
        virtual void onDownButtonPress() = 0;
};

#endif //CONTROLLERSBASE_H
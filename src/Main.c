#include <stdlib.h>
#include "numerical_integration/numint.h"

int main(int argc, char** argv) {

    // Call numint's interface.
    numint_interface(argc, argv);

    // End line. The app was success.
    return EXIT_SUCCESS;
}

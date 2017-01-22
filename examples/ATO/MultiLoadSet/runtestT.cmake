# 1. Run the program and generate the exodus output

EXECUTE_PROCESS(COMMAND ${TEST_PROG} nodal_ocT.xml RESULT_VARIABLE HAD_ERROR)

if(HAD_ERROR)
	message(FATAL_ERROR "Albany didn't run: test failed")
endif()

if(HAD_ERROR)
	message(FATAL_ERROR "Test failed")
endif()

EXECUTE_PROCESS(COMMAND ${TEST_PROG} nodal_nlopt.xml RESULT_VARIABLE HAD_ERROR)
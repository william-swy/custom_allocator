#include "CppUTest/TestHarness.h"
#include "CppUTestExt/MockSupport.h"
#include "lkl_malloc.c"

//TEST_GROUP(MockDocumentation){
//  void teardown(){
//    mock().clear();
//}
//}
//;
//
//void productionCode()
//{
//  mock().actualCall("productionCode");
//}
//
//TEST(MockDocumentation, SimpleScenario)
//{
//  mock().expectOneCall("productionCode");
//  productionCode();
//  mock().checkExpectations();
//}

TEST_GROUP(lkl_malloc_test){

};

TEST(lkl_malloc_test, AllocateZero)
{
  CHECK(lkl_malloc(0) == NULL);
}
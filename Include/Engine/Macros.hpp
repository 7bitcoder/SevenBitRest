#pragma once

#include "Engine/FromHeader.hpp"
#include "Engine/FromQuery.hpp"
#include "Engine/FromRoute.hpp"

#define FROM_QUERY(name) FromQuery<#name> name
#define FROM_ROUTE(name) FromRoute<#name> name
#define FROM_HEADER(name) FromHeader<#name> name
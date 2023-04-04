# Middlewares

## Default Middlewares

There are two special middlewares provided by the framework: Router and endpoints, which are automatically added to the middleware chain if not added manually (router is added as first middleware and endpoint as last one in the chain).

To add these middlewares use two functions: useRouter() and useEndpoints(), in following example these function calls could be omitted due to default behaviour.

<!-- MARKDOWN-AUTO-DOCS:START (CODE:src=../Examples/Middlewares/Default.cpp) -->
<!-- MARKDOWN-AUTO-DOCS:END -->

## Lambda Middleware

Instead of creating new middleware class, lambda function can be provided to use method.

<!-- MARKDOWN-AUTO-DOCS:START (CODE:src=../Examples/Middlewares/Lambda.cpp) -->
<!-- MARKDOWN-AUTO-DOCS:END -->

## Example

This example shows how to use middleware to add custom http header to response.

<!-- MARKDOWN-AUTO-DOCS:START (CODE:src=../Examples/Middlewares/AddHeaderToResponse.cpp) -->
<!-- MARKDOWN-AUTO-DOCS:END -->
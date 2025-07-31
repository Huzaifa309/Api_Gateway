#!/bin/bash

echo "Testing API Gateway with identity data..."

# Send POST request to the API
curl -X POST http://localhost:8080/data \
  -H "Content-Type: application/json" \
  -d @test_identity.json \
  -v

echo ""
echo "Test completed." 
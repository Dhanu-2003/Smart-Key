from flask import Flask, request, jsonify

app = Flask(__name__)

@app.route('/postdata', methods=['POST'])
def post_data():
    data = request.json  # Parse the incoming JSON data
    if not data:
        return jsonify({"error": "No data received"}), 400

    # Print the received data (for debugging)
    print(f"Received data: {data}")

    # Respond with a success message
    return jsonify({"message": "Data received successfully", "received": data}), 200

if __name__ == '__main__':
    # Make sure to use host='0.0.0.0' to allow connections from other devices on the network
    app.run(host='0.0.0.0', port=5000, debug=True)



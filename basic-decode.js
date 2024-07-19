// Decode uplink function.
function decodeUplink(input) {
    return {
        data: Decoder(input.bytes, input.fPort) // Call the Decoder function and return the decoded data
    };
}


// Encode downlink function.
// Function to decode the byte array into a readable string
function Decoder(bytes, port) {
    var result = "";
    // Iterate over each byte in the array
    for (var i = 0; i < bytes.length; i++) {
        // Convert each byte to a character and append to the result string
        result += String.fromCharCode(parseInt(bytes[i]));
    }

    // Return the decoded payload as an object
    return { payload: result, };
}

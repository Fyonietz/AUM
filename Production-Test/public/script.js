function login() {
    // Get values from input fields
    const nama = document.querySelector('input[name="name"]').value;
    const password = document.querySelector('input[name="Password"]').value;

    // Create data object to send
    const data = {
        nama: nama,
        password: password
    };

    // Send POST request to /auth
    fetch('/auth', {
        method: 'POST',
        headers: {
            'Content-Type': 'application/json'
        },
        body: JSON.stringify(data)
    })
    .then(response => response.json())
    .then(result => {
        console.log('Server response:', result);
    })
    .catch(error => {
        console.error('Error during login:', error);
    });
}

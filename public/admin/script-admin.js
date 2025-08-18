function save_user() {
    const data = {
        nama: document.getElementById('nama').value,
        password: document.getElementById('password').value,
        kelas: document.getElementById('Kelas').value
    };

    fetch('/api/register/user', {
        method: 'POST',
        headers: {
            'Content-Type': 'application/json'
        },
        body: JSON.stringify(data)
    })
    .then(response => {
        if (!response.ok) {
            throw new Error("Network response was not ok");
        }
        return response.json();
    })
    .then(result => {
        console.log('Success:', result);
        alert("User registered successfully!");
    })
    .catch(error => {
        console.error('Error:', error);
        alert("Failed to register user.");
    });
}
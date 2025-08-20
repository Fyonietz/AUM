function save_user() {
    const data = {
        nama: document.getElementById('nama').value,
        password: document.getElementById('password').value,
        kelas: document.getElementById('Kelas').value
    };
    if (!data.nama || !data.kelas || !data.password) {
        alert("Please fill in all required fields.");
        return; // stop function if any field is empty
    }

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
function toggle_ui(){
document.getElementById("toggleRegister").addEventListener("click", function () {
  const form = document.getElementById("registerForm");
  const reg_user = document.getElementsByClassName("register-user");

  form.classList.toggle("active");

  // Toggle a background-changing class on each .register-user element
  Array.from(reg_user).forEach((element) => {
    element.classList.toggle("bg-toggled");
  });
});
}
toggle_ui()

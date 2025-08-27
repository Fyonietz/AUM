function save_user() {
      const data = {
        nama: document.getElementById('nama').value,
        password: document.getElementById('password').value,
        kelas: document.getElementById('Kelas').value
      };

      if (!data.nama || !data.kelas || !data.password) {
        alert("Please fill in all required fields.");
        return;
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

    function toggle_ui() {
      const registerForm = document.getElementById("registerForm");
      const registerUser = document.querySelector(".register-user");
      registerForm.classList.toggle("active");
      registerUser.classList.toggle("bg-toggled");
    }

    document.addEventListener("DOMContentLoaded", function () {
      const toggleRegister = document.getElementById("toggleRegister");
      const toggleDaftar = document.getElementById("toggleDaftar");
      const registerForm = document.getElementById("registerForm");
      const registerUser = document.querySelector(".register-user");
      const iframeContainer = document.getElementById("iframeContainer");

      toggleRegister.addEventListener("click", () => {
        registerForm.classList.toggle("active");
        registerUser.classList.toggle("bg-toggled");
      });

      toggleDaftar.addEventListener("click", () => {
        iframeContainer.classList.toggle("active");
      });
    });
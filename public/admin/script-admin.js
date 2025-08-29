document.addEventListener("DOMContentLoaded", () => {
  // Delegate click event to all toggle buttons
  document.querySelectorAll('.toggle-btn').forEach(btn => {
    btn.addEventListener('click', () => {
      const targetId = btn.getAttribute('data-target');
      const bgClass = btn.getAttribute('data-bg');
      const targetEl = document.getElementById(targetId);
      const bgContainer = bgClass ? document.querySelector(`.${bgClass}`) : null;
      const toggleDaftar = document.getElementById("toggleDaftar");
      const iframeContainer = document.getElementById("iframeContainer");
      const iframe = iframeContainer.querySelector("iframe");
      // Toggle active class on the target container
      targetEl.classList.toggle('active');
      toggleDaftar.addEventListener("click", () => {
    iframeContainer.classList.toggle("active");

    // Reload iframe only if container is now visible
    if (iframeContainer.classList.contains("active")) {
      const currentSrc = iframe.src;
      iframe.src = "";      // reset src to force reload
      iframe.src = currentSrc;
    }
  });
      // Toggle background color if applicable
      if (bgContainer) {
        bgContainer.classList.toggle('bg-toggled');
      }
    });
  });
});

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
 async function deleteUser() {
    const nameToDelete = document.getElementById('searchName').value.trim();
    const messageElement = document.getElementById('deleteMessage');

    messageElement.textContent = "Mencari akun...";
    messageElement.className = "loading";

    if (!nameToDelete) {
      messageElement.textContent = "Nama harus diisi.";
      messageElement.className = "error";
      return;
    }

    try {
      // Fetch user list
      const response = await fetch('/api/user');
      const users = await response.json();

      // Simulate small delay (optional UX improvement)
      await new Promise(resolve => setTimeout(resolve, 500));

      const user = users.find(u => u.nama.trim().toLowerCase() === nameToDelete.toLowerCase());

      if (!user) {
        messageElement.textContent = "User tidak ditemukan.";
        messageElement.className = "error";
        return;
      }

      // Show success message that user was found
      messageElement.textContent = `Akun "${user.nama}" ditemukan. Menghapus...`;
      messageElement.className = "success";

      // Send POST request to delete the user
      const deleteResponse = await fetch('/api/delete/user', {
        method: 'POST',
        headers: {
          'Content-Type': 'application/json'
        },
        body: JSON.stringify({
          nama: user.nama,
          kelas: user.kelas,
          password: user.password
        })
      });

      if (deleteResponse.ok) {
        messageElement.textContent = `Akun "${user.nama}" berhasil dihapus.`;
        messageElement.className = "success";
      } else {
        messageElement.textContent = `Gagal menghapus akun "${user.nama}".`;
        messageElement.className = "error";
      }

    } catch (err) {
      console.error(err);
      messageElement.textContent = "Terjadi kesalahan saat menghapus user.";
      messageElement.className = "error";
    }
  }
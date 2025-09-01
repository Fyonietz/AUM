function login() {
    const nama = document.querySelector('input[name="name"]').value;
    const password = document.querySelector('input[name="Password"]').value;

    const data = {
        nama: nama,
        password: password
    };

    fetch('/auth', {
        method: 'POST',
        headers: {
            'Content-Type': 'application/json'
        },
        body: JSON.stringify(data)
    })
    .then(response => response.json()
        .then(json => ({
            ok: response.ok,
            status: response.status,
            body: json
        }))
    )
    .then(result => {
        if (result.ok && result.body.success) {
            window.location.href = result.body.redirect || "/admin";
        } else {
            // Show error alert
            document.getElementById("login-error").innerText = result.body.message || "Login failed.";
        }
    })
    .catch(error => {
        console.error('Error during login:', error);
    });
}

function Save_Kategori(){
    const Kategori = document.querySelector('input[name="Kategori-AUM"]').value.trim();
    const Id_Kategori = document.querySelector('input[name="Id-Kategori-AUM"]').value.trim();


    fetch('api/save',{
        method:'POST',
        headers:{
            'Content-Type' : 'text/plain'
        },
        body:`${Kategori},${Id_Kategori}`

    })
}

 fetch('/api/read')
      .then(response => {
        return response.json();
      })
      .then(data => {
        const tbody = document.getElementById('data-table-body');
        tbody.innerHTML = ''; // Clear table body

        data.forEach(item => {
          const row = document.createElement('tr');

          const idCell = document.createElement('td');
          idCell.textContent = item.id;

          const nameCell = document.createElement('td');
          nameCell.textContent = item.nama_soal_masalah;

          row.appendChild(idCell);
          row.appendChild(nameCell);
          tbody.appendChild(row);
        });
      })
      .catch(error => {
        console.error('Error:', error);
      });

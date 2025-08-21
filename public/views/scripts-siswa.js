let currentUser = null; // global to keep user info

async function loadUserInfo() {
  try {
    const res = await fetch('/whoiam');
    if (!res.ok) throw new Error('Unauthorized');
    currentUser = await res.json();

    const welcomeEl = document.getElementById('welcome');
    welcomeEl.textContent = `Welcome, ${currentUser.nama} (${currentUser.kelas})`;
  } catch (err) {
    console.error('User fetch error:', err);
    document.getElementById('welcome').textContent = 'Welcome, Guest';
    currentUser = { nama: 'Guest', kelas: '' }; // fallback user object
  }
  return currentUser;
}

async function loadData() {
  try {
    const response = await fetch('/api/read');
    if (!response.ok) throw new Error('Network response was not ok');
    const data = await response.json();

    // Group data by category (optional)
    const grouped = data.reduce((acc, item) => {
      if (!acc[item.nama_bidang_masalah]) {
        acc[item.nama_bidang_masalah] = [];
      }
      acc[item.nama_bidang_masalah].push(item);
      return acc;
    }, {});

    const container = document.getElementById('checkboxContainer');
    container.innerHTML = '';

    let index = 1; // Initialize the continuous index

    for (const [, problems] of Object.entries(grouped)) {
      problems.forEach(problem => {
        const wrapper = document.createElement('div');
        wrapper.classList.add('checkbox-item');

        const checkbox = document.createElement('input');
        checkbox.type = 'checkbox';
        checkbox.name = 'problems';
        checkbox.value = problem.id;
        checkbox.id = `problem-${problem.id}`;

        const label = document.createElement('label');
        label.htmlFor = checkbox.id;
        // Display the continuous index instead of the problem ID
        label.textContent = ` ${index++}. ${problem.nama_soal_masalah}`;

        checkbox.addEventListener('change', function () {
          wrapper.classList.toggle('checked', this.checked);
        });

        wrapper.appendChild(checkbox);
        wrapper.appendChild(label);
        container.appendChild(wrapper);
      });
    }

    window.surveyData = data; // store full data globally

  } catch (error) {
    document.getElementById('checkboxContainer').textContent = 'Failed to load questions.';
    console.error('Fetch error:', error);
  }
}


document.getElementById('surveyForm').addEventListener('submit', function (event) {
  event.preventDefault();

  if (!currentUser) {
    alert('User info not loaded yet!');
    return;
  }

  const checkedBoxes = document.querySelectorAll('input[name="problems"]:checked');
  const combinedArray = Array.from(checkedBoxes).map(cb => {
    const problem = window.surveyData.find(item => item.id === cb.value);
    return {
      nama: currentUser.nama,
      kelas: currentUser.kelas,
      id: problem.id,
      nama_bidang_masalah: problem.nama_bidang_masalah
    };
  });

  // Show the combined array in the result element for debugging
  document.getElementById('result').textContent = JSON.stringify(combinedArray, null, 2);

  // Send the combinedArray via POST request
  fetch('/aum/submit', {
    method: 'POST',
    headers: {
      'Content-Type': 'application/json'
    },
    body: JSON.stringify(combinedArray)
  })
  .then(response => {
    // Check if the response is okay
    if (!response.ok) {
      throw new Error(`HTTP error! status: ${response.status}`);
    }
    // Parse the JSON response
    return response.json();
  })
  .then(json => {
    // Handle the JSON response here
    console.log('Response JSON:', json);
    console.log('Status:', json.status); // Example: handle status if available
    console.log('Body:', json.body); // Example: handle body if available
  })
  .catch(error => {
    console.error('Error submitting survey data:', error);
  });
});


window.onload = async () => {
  await loadUserInfo(); // wait for user info
  await loadData();     // then load problems
};


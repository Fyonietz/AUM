let currentUser = null;
let currentPage = 1;
const itemsPerPage = 30;
let groupedData = {}; // for pagination
let allData = [];     // for global access
let selectedProblems = new Set();
// Load user info from the API
async function loadUserInfo() {
  try {
    const res = await fetch('/whoiam');
    if (!res.ok) throw new Error('Unauthorized');
    currentUser = await res.json();
    document.getElementById('welcome').textContent = `${currentUser.nama} (${currentUser.kelas})`;
  } catch (err) {
    console.error('User fetch error:', err);
    document.getElementById('welcome').textContent = 'Guest';
    currentUser = { nama: 'Guest', kelas: '' };
  }
}

// Load the data (questions) from the API
async function loadData() {
  try {
    const response = await fetch('/api/read');
    if (!response.ok) throw new Error('Failed to load questions.');
    const data = await response.json();

    // Store full data
    allData = data;
    window.surveyData = data;

    // Group by category
    groupedData = data.reduce((acc, item) => {
      if (!acc[item.nama_bidang_masalah]) acc[item.nama_bidang_masalah] = [];
      acc[item.nama_bidang_masalah].push(item);
      return acc;
    }, {});

    renderPage(currentPage);
    renderPaginationControls();

  } catch (error) {
    console.error('Error loading data:', error);
    document.getElementById('checkboxContainer').textContent = 'Failed to load questions.';
  }
}
function renderPage(page) {
  const container = document.getElementById('checkboxContainer');
  container.innerHTML = ''; // Clear existing content

  const flatList = Object.values(groupedData).flat();
  const totalPages = Math.ceil(flatList.length / itemsPerPage);
  const start = (page - 1) * itemsPerPage;
  const end = start + itemsPerPage;
  const pageItems = flatList.slice(start, end);

  let index = start + 1;

  pageItems.forEach(problem => {
    const wrapper = document.createElement('div');
    wrapper.classList.add('checkbox-item');

    const checkbox = document.createElement('input');
    checkbox.type = 'checkbox';
    checkbox.name = 'problems';
    checkbox.value = problem.id;
    checkbox.id = `problem-${problem.id}`;

    // Restore checkbox state from selectedProblems
    if (selectedProblems.has(problem.id.toString())) {
      checkbox.checked = true;
      wrapper.classList.add('checked');
    }

    checkbox.addEventListener('change', function () {
      if (this.checked) {
        selectedProblems.add(this.value);
        wrapper.classList.add('checked');
      } else {
        selectedProblems.delete(this.value);
        wrapper.classList.remove('checked');
      }
    });

    const label = document.createElement('label');
    label.htmlFor = checkbox.id;
    label.textContent = `${index++}. ${problem.nama_soal_masalah}`;

    wrapper.appendChild(checkbox);
    wrapper.appendChild(label);
    container.appendChild(wrapper);
  });

  // Show submit button only on the last page
  const submitBtn = document.querySelector('.submit-btn');
  if (submitBtn) {
    submitBtn.style.display = page === totalPages ? 'block' : 'none';
  }

  // Scroll to the top when changing pages
  window.scrollTo(0, 0);
}


// Function to render pagination controls (page buttons)
function renderPaginationControls() {
  const flatList = Object.values(groupedData).flat();
  const totalPages = Math.ceil(flatList.length / itemsPerPage);

  const container = document.getElementById('paginationControls');
  container.innerHTML = '';

  if (totalPages <= 1) return;

  // Create Prev button
  const prevBtn = document.createElement('button');
  prevBtn.textContent = '« Prev';
  prevBtn.disabled = currentPage === 1;
  prevBtn.onclick = () => {
    currentPage--;
    renderPage(currentPage);
    renderPaginationControls();
  };
  container.appendChild(prevBtn);

  // Create page buttons
  for (let i = 1; i <= totalPages; i++) {
    const pageBtn = document.createElement('button');
    pageBtn.textContent = i;
    pageBtn.disabled = i === currentPage;
    pageBtn.onclick = () => {
      currentPage = i;
      renderPage(currentPage);
      renderPaginationControls();
    };
    container.appendChild(pageBtn);
  }

  // Create Next button
  const nextBtn = document.createElement('button');
  nextBtn.textContent = 'Next »';
  nextBtn.disabled = currentPage === totalPages;
  nextBtn.onclick = () => {
    currentPage++;
    renderPage(currentPage);
    renderPaginationControls();
  };
  container.appendChild(nextBtn);
}
document.getElementById('surveyForm').addEventListener('submit', function (event) {
  event.preventDefault();

  if (!currentUser) {
    alert('User info not loaded yet!');
    return;
  }

  const combinedArray = Array.from(selectedProblems).map(id => {
    const problem = allData.find(item => item.id.toString() === id);
    return {
      nama: currentUser.nama,
      kelas: currentUser.kelas,
      id: problem.id,
      nama_bidang_masalah: problem.nama_bidang_masalah
    };
  });
  console.log("Data to submit:", JSON.stringify(combinedArray, null, 2));

  fetch('/aum/submit', {
    method: 'POST',
    headers: {
      'Content-Type': 'application/json'
    },
    body: JSON.stringify(combinedArray)
  })
  .then(response => {
    if (!response.ok) throw new Error(`HTTP error! status: ${response.status}`);
    return response.json();
  })
  .then(json => {
    console.log('Response JSON:', json);
    alert('Survey submitted successfully!');
  })
  .catch(error => {
    console.error('Error submitting survey data:', error);
    alert('Submission failed.');
  });
});

// Init
window.onload = async () => {
  await loadUserInfo();
  await loadData();
};

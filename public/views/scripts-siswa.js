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

    // Render checkboxes (without showing categories)
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
        label.textContent = ` ${problem.id}. ${problem.nama_soal_masalah}`;

        // Add stretch effect on check
        checkbox.addEventListener('change', function () {
          wrapper.classList.toggle('checked', this.checked);
        });

        wrapper.appendChild(checkbox);
        wrapper.appendChild(label);
        container.appendChild(wrapper);
      });
    }

    // Save full data globally
    window.surveyData = data;

  } catch (error) {
    document.getElementById('checkboxContainer').textContent = 'Failed to load questions.';
    console.error('Fetch error:', error);
  }
}

document.getElementById('surveyForm').addEventListener('submit', function (event) {
  event.preventDefault();

  const checkedBoxes = document.querySelectorAll('input[name="problems"]:checked');
  const selectedProblems = Array.from(checkedBoxes).map(cb =>
    window.surveyData.find(item => item.id === cb.value)
  );

  document.getElementById('result').textContent = JSON.stringify(selectedProblems, null, 2);
});

window.onload = loadData;

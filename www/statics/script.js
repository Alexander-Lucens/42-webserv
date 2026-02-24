const themeToggle = document.getElementById('theme-toggle');
const currentTheme = localStorage.getItem('theme') || 'dark';

document.documentElement.setAttribute('data-theme', currentTheme);

themeToggle.addEventListener('click', () => {
	let theme = document.documentElement.getAttribute('data-theme');
	let newTheme = theme === 'dark' ? 'light' : 'dark';
	document.documentElement.setAttribute('data-theme', newTheme);
	localStorage.setItem('theme', newTheme);
});

document.querySelectorAll(".nav-btn").forEach(button => {
	button.addEventListener("mousemove", e => {
		const rect = button.getBoundingClientRect();
		const x = e.clientX - rect.left;
		const y = e.clientY - rect.top;
		button.style.setProperty("--x", `${x}px`);
		button.style.setProperty("--y", `${y}px`);
	});
});
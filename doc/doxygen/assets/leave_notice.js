function getRootDomain(hostname) {
	return hostname.split(".").slice(-2).join(".");
}

document.addEventListener("click", function (e) {
	const link = e.target.closest("a");
	if (!link) return;

	const href = link.getAttribute("href");
	if (!href) return;

	try {
		const url = new URL(href, window.location.href);
		if (getRootDomain(url.hostname) !== getRootDomain(window.location.hostname)) {
			const confirmed = confirm(
				`You are leaving this site and going to:\n${url.href}\n\nDo you want to continue?`
			);
			if (!confirmed) e.preventDefault();
		}
	} catch {
		// Ignore malformed URLs
	}
});

import Section from "../ui/Section";
import { GithubIcon } from "../icons/GithubIcon";
import { GlobeIcon } from "../icons/GlobeIcon";

const projects = [
    {
        name: "tadeusz",
        title: "An AI assistant",
    },
    {
        name: "neoarchlinux.org",
        owner: "neoarchlinux",
        title: "NeoArch Linux website",
        url: "https://neoarchlinux.org"
    },
    {
        name: "napm",
        owner: "neoarchlinux",
        title: "NeoArch Package Manager",
        url: "https://docs.neoarchlinux.org/napm"
    },
    {
        name: "FusionSearch",
        title: "A search engine based on the idea of SearX",
        url: "https://fusion.doleckijakub.pl"
    }
];

export default function Projects() {
    return (
        <Section title="projects">
            <div className="grid gap-4">
                {projects.map((p) => (
                    <div key={p.name} className="border border-white/10 p-4 rounded">
                        <div className="grid gap-2 items-center grid-cols-1 sm:grid-cols-[minmax(120px,200px)_1fr_auto]">
                            {/* name */}
                            <span className="font-medium">{p.name}</span>

                            {/* title */}
                            <span className="text-white/70 sm:truncate">{p.title}</span>

                            {/* actions */}
                            <div className="flex gap-2 sm:justify-end">
                                {p.url && (
                                    <a href={p.url} target="_blank" rel="noopener noreferrer">
                                        <GlobeIcon className="w-5 h-5 text-white" />
                                    </a>
                                )}

                                <a href={"https://github.com/" + (p.owner || "doleckijakub") + "/" + p.name} target="_blank" rel="noopener noreferrer">
                                    <GithubIcon className="w-5 h-5 text-white" />
                                </a>
                            </div>
                        </div>

                    </div>
                ))}
            </div>
        </Section>
    );
}
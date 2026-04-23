import Hero from "@/components/blocks/Hero";
import Projects from "@/components/blocks/Projects";
import TechStack from "@/components/blocks/TechStack";
import GithubStats from "@/components/blocks/GithubStats";

export default function Home() {
  return (
    <main className="space-y-20 py-10">
      <Hero />
      <Projects />
      <TechStack />
      {/* TODO: Education */}
      <GithubStats />
      {/* TODO: BlogPreview */}
      {/* TODO: ServerStats */}
    </main>
  );
}